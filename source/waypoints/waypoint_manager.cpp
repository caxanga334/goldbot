#include <extdll.h>
#include <meta_api.h>

// undef these to use fstream
#undef open
#undef read
#undef write
#undef close

#include <fstream>
#include <filesystem>
#include <cstring>
#include <cfloat> // Linux, for FLT_MAX

#include "manager.h"
#include "pluginplayer.h"
#include "mods/mod_base.h"
#include "interfaces/player.h"
#include "interfaces/pluginbot.h"
#include "bots/basebot.h"
#include "waypoint_base.h"
#include "waypoint_manager.h"
#include "waypoint_pathfind.h"

class CWaypointFileHeader
{
public:
	char header[10];
	int version;
	int subversion;
	int num_waypoints;
};


CWaypointManager* TheWaypoints = nullptr;

CWaypointManager::CWaypointManager() : 
	m_spriteTexture(0),
	m_editupdatetimer(0.0f),
	m_authpathdist(0.0f),
	m_editmode(false),
	m_loaded(false),
	m_pathstart(-1),
	m_waypoints(),
	m_editor()
{
	m_waypoints.clear();
}

CWaypointManager::~CWaypointManager()
{
	for (auto waypoint : m_waypoints)
	{
		delete waypoint;
	}

	m_waypoints.clear();
}

void CWaypointManager::EditingUpdate()
{
	if (m_editmode && m_editor.IsValid() && m_editupdatetimer <= gpGlobals->time)
	{
		m_editupdatetimer = gpGlobals->time + 1.0f;
		Vector origin(m_editor->v.origin);
		
		for (auto waypoint : m_waypoints)
		{
			if (waypoint->DistToSqr(origin) <= WaypointConst::WaypointDrawDistance)
			{
				waypoint->Draw();
			}
		}

		CWaypoint* nearest = GetNearestWaypoint(origin, WaypointConst::WaypointDrawDistance);

		if (nearest != nullptr)
		{
			nearest->DrawPath();
		}
	}
}

void CWaypointManager::OnMapChange()
{
	m_loaded = false;
	m_editupdatetimer = -1.0f;
	m_editmode = false;
	m_pathstart = WaypointConst::InvalidPathConnection;
	m_authpathdist = WaypointConst::AuthPathDistance;
	Load();
}

bool CWaypointManager::ClientCommand(IPlayer* player, const char* command, const std::vector<const char*>& args, const int argc)
{
	if (strcmp(command, "gb") == 0)
	{
		auto client = VARS(player->GetEdict());

		if (argc == 0)
		{
			ClientPrint(client, HUD_PRINTCONSOLE, "Usage: gb [subcommand] [args...] \nType: gb help for more. \n");
			return true;
		}

		if (argc == 1) // commands with 1 argument
		{
			if (strcmp(args[0], "help") == 0)
			{
				ClientPrint(client, HUD_PRINTCONSOLE, "Goldbot client command list: \n");
				ClientPrint(client, HUD_PRINTCONSOLE, "gb help - Prints a list of commands \n");
				ClientPrint(client, HUD_PRINTCONSOLE, "gb waypoint - Prints all waypoint editing commands \n");
				ClientPrint(client, HUD_PRINTCONSOLE, "gb settings - Client controlled settings \n");
				return true;
			}
			else if (strcmp(args[0], "waypoint") == 0)
			{
				ClientPrint(client, HUD_PRINTCONSOLE, "Goldbot waypoint editing commands: \n");
				ClientPrint(client, HUD_PRINTCONSOLE, "becomeeditor addwaypoint save info createpath createpathboth deletepath deletepathboth info setpathtype setpathtypeboth \n");
				ClientPrint(client, HUD_PRINTCONSOLE, "Note: Waypoint editing must be enabled on the server. \n");
				return true;
			}
		}

		if (argc >= 2)
		{
			if (strcmp(args[0], "waypoint") == 0 && TheWaypoints->IsEditing()) // Waypoint editing commands
			{
				if (strcmp(args[1], "becomeeditor") == 0)
				{
					if (m_editor == player->GetEdict())
					{
						m_editor.Invalidate();
						ClientPrint(client, HUD_PRINTCONSOLE, "You have released your editing rights! \n");
						return true;
					}

					m_editor = player->GetEdict();
					ClientPrint(client, HUD_PRINTCONSOLE, "You now have editing rights! \n");
					return true;
				}

				if (m_editor != player->GetEdict())
				{
					ClientPrint(client, HUD_PRINTCONSOLE, "You don't have editing rights! \n Use gb waypoint becomeeditor \n ");
					return true;
				}

				if (strcmp(args[1], "addwaypoint") == 0)
				{
					CreateWaypoint(player);
					ClientPrint(client, HUD_PRINTCONSOLE, "Waypoint added! \n");
					return true;
				}
				else if (strcmp(args[1], "deletewaypoint") == 0)
				{
					CWaypoint* waypoint = GetNearestWaypoint(player->GetPosition(), 128.0f * 128.0f);

					if (waypoint == nullptr)
					{
						ClientPrint(client, HUD_PRINTCONSOLE, "No waypoint nearby! \n");
						return true;
					}

					DeleteWaypoint(waypoint);
					ClientPrint(client, HUD_PRINTCONSOLE, "Waypoint deleted! \n");
					return true;
				}
				else if (strcmp(args[1], "save") == 0)
				{
					Save();
					ClientPrint(client, HUD_PRINTCONSOLE, "Waypoints saved! \n");
					return true;
				}
				else if (strcmp(args[1], "createpathboth") == 0)
				{
					auto wpt = GetNearestWaypoint(player->GetPosition());

					if (wpt != nullptr)
					{
						if (m_pathstart == WaypointConst::InvalidPathConnection)
						{
							m_pathstart = wpt->GetID();
							ClientPrint(client, HUD_PRINTCONSOLE, "First waypoint set. Now go to the target waypoint and use this command again! \n");
							EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/button3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						}
						else
						{
							auto start = GetWaypointOfID(m_pathstart);
							m_pathstart = WaypointConst::InvalidPathConnection;
							bool result = start->AddPathTo(wpt) && wpt->AddPathTo(start);

							if (!result)
							{
								ClientPrint(client, HUD_PRINTCONSOLE, "Failed to create a path between waypoints \n");
							}
							else
							{
								EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
								ClientPrint(client, HUD_PRINTCONSOLE, "Path created! \n");
							}
						}
					}

					return true;
				}
				else if (strcmp(args[1], "createpath") == 0)
				{
					auto wpt = GetNearestWaypoint(player->GetPosition());

					if (wpt != nullptr)
					{
						if (m_pathstart == WaypointConst::InvalidPathConnection)
						{
							m_pathstart = wpt->GetID();
							ClientPrint(client, HUD_PRINTCONSOLE, "First waypoint set. Now go to the target waypoint and use this command again! \n");
						}
						else
						{
							auto start = GetWaypointOfID(m_pathstart);
							m_pathstart = WaypointConst::InvalidPathConnection;
							bool result = start->AddPathTo(wpt);

							if (!result)
							{
								ClientPrint(client, HUD_PRINTCONSOLE, "Failed to create a path between waypoints \n");
							}
							else
							{
								EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
								ClientPrint(client, HUD_PRINTCONSOLE, "Path created! \n");
							}
						}
					}

					return true;
				}
				else if (strcmp(args[1], "deletepathboth") == 0)
				{
					auto wpt = GetNearestWaypoint(player->GetPosition());

					if (wpt != nullptr)
					{
						if (m_pathstart == WaypointConst::InvalidPathConnection)
						{
							m_pathstart = wpt->GetID();
							ClientPrint(client, HUD_PRINTCONSOLE, "First waypoint set. Now go to the target waypoint and use this command again! \n");
							EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/button3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						}
						else
						{
							auto start = GetWaypointOfID(m_pathstart);
							m_pathstart = WaypointConst::InvalidPathConnection;
							bool result = start->DeletePathTo(wpt) && wpt->DeletePathTo(start);

							if (!result)
							{
								ClientPrint(client, HUD_PRINTCONSOLE, "Failed to delete a path between waypoints \n");
							}
							else
							{
								EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
								ClientPrint(client, HUD_PRINTCONSOLE, "Path deleted! \n");
							}
						}
					}

					return true;
				}
				else if (strcmp(args[1], "deletepath") == 0)
				{
					auto wpt = GetNearestWaypoint(player->GetPosition());

					if (wpt != nullptr)
					{
						if (m_pathstart == WaypointConst::InvalidPathConnection)
						{
							m_pathstart = wpt->GetID();
							ClientPrint(client, HUD_PRINTCONSOLE, "First waypoint set. Now go to the target waypoint and use this command again! \n");
							EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/button3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						}
						else
						{
							auto start = GetWaypointOfID(m_pathstart);
							m_pathstart = WaypointConst::InvalidPathConnection;
							bool result = start->DeletePathTo(wpt);

							if (!result)
							{
								ClientPrint(client, HUD_PRINTCONSOLE, "Failed to delete a path between waypoints \n");
							}
							else
							{
								EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
								ClientPrint(client, HUD_PRINTCONSOLE, "Path deleted! \n");
							}
						}
					}

					return true;
				}
				else if (strcmp(args[1], "info") == 0)
				{
					auto wpt = GetNearestWaypoint(player->GetPosition());

					if (wpt != nullptr)
					{
						EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						wpt->PrintWaypointInfo(player);
					}

					return true;
				}
				else if (strcmp(args[1], "setpathtypeboth") == 0)
				{
					if (argc < 3)
					{
						ClientPrint(client, HUD_PRINTCONSOLE, "Usage: gb waypoint setpathtype <type>\n");
						ClientPrint(client, HUD_PRINTCONSOLE, "Valid types: normal jump (gap/gapjump) (stairs/ramp) ladder");
						return true;
					}

					auto wpt = GetNearestWaypoint(player->GetPosition());
					auto type = WaypointPath::NameToType(args[2]);

					if (wpt != nullptr)
					{
						if (m_pathstart == WaypointConst::InvalidPathConnection)
						{
							m_pathstart = wpt->GetID();
							ClientPrint(client, HUD_PRINTCONSOLE, "First waypoint set. Now go to the target waypoint and use this command again! \n");
							EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/button3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						}
						else
						{
							auto start = GetWaypointOfID(m_pathstart);
							m_pathstart = WaypointConst::InvalidPathConnection;
							bool result = start->SetPathTypeTo(wpt, type) && wpt->SetPathTypeTo(start, type);

							if (!result)
							{
								ClientPrint(client, HUD_PRINTCONSOLE, "Failed to change path type between waypoints \n");
							}
							else
							{
								EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
								ClientPrint(client, HUD_PRINTCONSOLE, "Path type updated! \n");
							}
						}
					}

					return true;
				}
				else if (strcmp(args[1], "setpathtype") == 0)
				{
					if (argc < 3)
					{
						ClientPrint(client, HUD_PRINTCONSOLE, "Usage: gb waypoint setpathtype <type>\n");
						ClientPrint(client, HUD_PRINTCONSOLE, "Valid types: normal jump (gap/gapjump) (stairs/ramp)");
						return true;
					}

					auto wpt = GetNearestWaypoint(player->GetPosition());
					auto type = WaypointPath::NameToType(args[2]);

					if (wpt != nullptr)
					{
						if (m_pathstart == WaypointConst::InvalidPathConnection)
						{
							m_pathstart = wpt->GetID();
							ClientPrint(client, HUD_PRINTCONSOLE, "First waypoint set. Now go to the target waypoint and use this command again! \n");
							EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/button3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
						}
						else
						{
							auto start = GetWaypointOfID(m_pathstart);
							m_pathstart = WaypointConst::InvalidPathConnection;
							bool result = start->SetPathTypeTo(wpt, type);

							if (!result)
							{
								ClientPrint(client, HUD_PRINTCONSOLE, "Failed to change path type between waypoints \n");
							}
							else
							{
								EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
								ClientPrint(client, HUD_PRINTCONSOLE, "Path type updated! \n");
							}
						}
					}

					return true;
				}
			}
			else if (strcmp(args[0], "settings") == 0)
			{
				if (TheWaypoints->IsEditing())
				{
					if (strcmp(args[1], "autopathdist") == 0)
					{
						if (argc < 3)
						{
							ClientPrint(client, HUD_PRINTCONSOLE, "Usage: gb settings autopathdist <number> \n");
							ClientPrint(client, HUD_PRINTCONSOLE, "Sets the distance which path are automatically created when placing new waypoints \n");
							return true;
						}

						float newdist = atof(args[2]);
						m_authpathdist = newdist;

						char buffer[512]{};
						sprintf(buffer, "Auth path distance set to %3.2f", newdist);
						ClientPrint(client, HUD_PRINTCONSOLE, buffer);
						EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "buttons/blip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

						if (IS_DEDICATED_SERVER() != 0)
						{
							LOG_CONSOLE(PLID, "Auth path distance changed by %s, set to %3.2f", STRING(player->GetEdict()->v.netname), newdist);
						}

						return true;
					}
				}

				ClientPrint(client, HUD_PRINTCONSOLE, "Usage: gb settings <settings> \n");
				ClientPrint(client, HUD_PRINTCONSOLE, "Available settings: autopathdist \n");
				return true;
			}
		}
	}

	return false;
}

CWaypoint* CWaypointManager::WaypointFactory()
{
	return new CWaypoint;
}

CWaypoint* CWaypointManager::CreateWaypoint(IPlayer* player)
{
	int id = 0;

	for (auto waypoint : m_waypoints)
	{
		if (id != waypoint->GetID()) // first the first available ID
		{
			break;
		}

		id++;
	}

	// The position vector must be a new object otherwise we will sink the waypointer into the ground
	Vector position = Vector(player->GetPosition());
	auto& eyes = player->GetEyeAngles();
	auto wpt = WaypointFactory();

	position[2] = position[2] - gamemod->GetPlayerOriginGroundOffset();

	wpt->Init(id ,position, eyes[YAW]);

	std::vector<CWaypoint*> nearbywpts;
	CollectWaypointsInRadius(position, m_authpathdist, nearbywpts);

	for (auto neighbor : nearbywpts)
	{
		wpt->AutoPathTo(neighbor);
		neighbor->AutoPathTo(wpt);
	}

	EMIT_SOUND_DYN(player->GetEdict(), CHAN_ITEM, "weapons/xbow_hit1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
	m_waypoints.push_back(wpt);

	if (IS_DEDICATED_SERVER() != 0)
	{
		LOG_CONSOLE(PLID, "Waypoint added by %s at <%3.2f, %3.2f, %3.2f>", STRING(player->GetEdict()->v.netname), position[0], position[1], position[2]);
	}

	return wpt;
}

bool CWaypointManager::DeleteWaypoint(CWaypoint* todelete)
{
	const int id = todelete->GetID();
	auto it = std::find(m_waypoints.begin(), m_waypoints.end(), todelete);

	if (it != m_waypoints.end())
	{
		m_waypoints.erase(it);
		delete todelete;

		for (auto waypoint : m_waypoints)
		{
			waypoint->NotifyWaypointDeletion(id);
		}

		EMIT_SOUND_DYN(m_editor.Get(), CHAN_ITEM, "weapons/mine_activate.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		return true;
	}

	return false;
}

void CWaypointManager::Save()
{
	std::fstream file;
	std::unique_ptr<char[]> gamedir(new char[256]);
	GET_GAME_DIR(gamedir.get());
	std::string gameroot = std::string(gamedir.get());
	std::string map = std::string(STRING(gpGlobals->mapname));
	std::string wptdir = std::string("/addons/goldbot/waypoints/");
	std::string dir = gameroot + wptdir + gamemod->GetModDataDirectory();
	std::string filename = dir + "/" + map + ".wpt";

	file.open(filename, std::fstream::out | std::fstream::binary | std::fstream::trunc);

	CWaypointFileHeader header{};
	int subversion = GetSubversion();

	strncpy(header.header, WAYPOINT_FILE_HEADER, 7);
	header.header[9] = 0;

	header.version = GetVersion();
	header.subversion = subversion;
	header.num_waypoints = static_cast<int>(m_waypoints.size());

	file.write(reinterpret_cast<char*>(&header), sizeof(CWaypointFileHeader));

	PreWaypointCustomSave(file, subversion);

	for (auto waypoint : m_waypoints)
	{
		waypoint->Save(file, GetVersion());
		waypoint->SaveCustom(file, subversion);
	}

	PostWaypointCustomSave(file, subversion);

	LOG_CONSOLE(PLID, "Waypoints saved! %s", filename.c_str());
	file.close();
}

bool CWaypointManager::Load()
{
	std::fstream file;
	std::unique_ptr<char[]> gamedir(new char[256]);
	GET_GAME_DIR(gamedir.get());
	std::string gameroot = std::string(gamedir.get());
	std::string map = std::string(STRING(gpGlobals->mapname));
	std::string wptdir = std::string("/addons/goldbot/waypoints/");
	std::string dir = gameroot + wptdir + gamemod->GetModDataDirectory();
	std::string filename = dir + "/" + map + ".wpt";


	// Clear waypoints (for file reloads)
	for (auto waypoint : m_waypoints)
	{
		delete waypoint;
	}

	m_waypoints.clear();

	file.open(filename, std::fstream::in | std::fstream::binary);

	if (file.is_open() == false || file.eof() == true)
	{
		LOG_CONSOLE(PLID, "Waypoints for \"%s\" not loaded. File not found!", map.c_str());
		if (file.is_open()) { file.close(); }
		return false;
	}

	CWaypointFileHeader header{};

	file.read(reinterpret_cast<char*>(&header), sizeof(CWaypointFileHeader));

	if (strncmp(header.header, WAYPOINT_FILE_HEADER, 7) != 0)
	{
		LOG_CONSOLE(PLID, "File \"%s\" is not a GoldBot waypoint file!", filename.c_str());
		file.close();
		return false;
	}

	if (header.version > GetVersion())
	{
		LOG_CONSOLE(PLID, "Invalid waypoint version for file \"%s\"!", filename.c_str());
		file.close();
		return false;
	}

	if (PreWaypointCustomLoad(file, header.subversion) == false)
	{
		file.close();
		return false;
	}

	for (int i = 0; i < header.num_waypoints; i++)
	{
		CWaypoint* wpt = WaypointFactory();

		if (wpt->Load(file, header.version) == false)
		{
			delete wpt;
			file.close();
			return false;
		}

		if (wpt->LoadCustom(file, header.subversion) == false)
		{
			delete wpt;
			file.close();
			return false;
		}

		m_waypoints.push_back(wpt);
	}

	if (PostWaypointCustomLoad(file, header.subversion) == false)
	{
		file.close();
		return false;
	}

	for (auto wpt : m_waypoints)
	{
		wpt->PostAllWaypointsLoaded();
	}
	
	LOG_MESSAGE(PLID, "Waypoints loaded successfully! \n Number of Waypoints: %i \n Version/Subversion: %i/%i ", 
		header.num_waypoints, header.version, header.subversion);
	file.close();
	m_loaded = true;
	return true;
}

void CWaypointManager::ToggleEditing()
{
	m_editmode = !m_editmode;

	if (m_editmode)
	{
		LOG_CONSOLE(PLID, "Waypoint editing enabled!");
	}
	else
	{
		LOG_CONSOLE(PLID, "Waypoint editing disabled!");
	}
}

CWaypoint* CWaypointManager::GetWaypointOfID(const int id)
{
	for (auto waypoint : m_waypoints)
	{
		if (waypoint->GetID() == id)
		{
			return waypoint;
		}
	}

	return nullptr;
}

CWaypoint* CWaypointManager::GetNearestWaypoint(const Vector& position)
{
	float nearest = FLT_MAX;
	float distance = 0.0f;
	CWaypoint* best = nullptr;

	for (auto waypoint : m_waypoints)
	{
		distance = waypoint->DistToSqr(position);

		if (distance < nearest)
		{
			best = waypoint;
			nearest = distance;
		}
	}

	return best;
}

CWaypoint* CWaypointManager::GetNearestWaypoint(const Vector& position, const float maxdistsqr)
{
	float nearest = FLT_MAX;
	float distance = 0.0f;
	CWaypoint* best = nullptr;

	for (auto waypoint : m_waypoints)
	{
		distance = waypoint->DistToSqr(position);

		if (distance <= maxdistsqr && distance < nearest)
		{
			best = waypoint;
			nearest = distance;
		}
	}

	return best;
}

int CWaypointManager::CollectWaypointsInRadius(const Vector& source, const float radius, std::vector<CWaypoint*>& waypointvector)
{
	int counter = 0;

	for (auto waypoint : m_waypoints)
	{
		if (waypoint->DistToSqr(source) <= radius)
		{
			waypointvector.push_back(waypoint);
			counter++;
		}
	}

	return counter;
}
