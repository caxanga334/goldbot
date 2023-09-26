
#include <extdll.h>
#include <meta_api.h>

#include "hl1valve/baseentity.h"

#include <memory>
#include <fstream>
#include <string>
#include <filesystem>

#include "pluginplayer.h"
#include "pluginglobals.h"
#include "plugincvars.h"
#include "pluginutil.h"
#include "interfaces/player.h"
#include "interfaces/pluginbot.h"
#include "mods/mod_base.h"
#include "waypoints/waypoint_manager.h"
#include "waypoints/waypoint_pathfind.h"
#include "manager.h"

CPluginBotManager* TheBotManager = nullptr;

CPluginBotManager::CPluginBotManager()
{
}

CPluginBotManager::~CPluginBotManager()
{
}

void CPluginBotManager::Update()
{
	gamemod->Update();

	for (auto bot : m_botlist)
	{
		bot->UpKeep(); // light-weight update every tick
		bot->BeginUpdate(); // Update the bot if it's time to
	}
}

void CPluginBotManager::CreatePluginDirectories(const char* gamedir)
{
	std::string rootdir(gamedir);
	std::string configdir("/addons/goldbot/configs");
	std::string wptdir("/addons/goldbot/waypoints/");
	std::string moddir = rootdir + configdir;

	std::filesystem::create_directories(moddir); // create config folder

	moddir = rootdir + wptdir + gamemod->GetModDataDirectory();

	std::filesystem::create_directories(moddir); // create waypoint folder
}

CPluginPlayer* CPluginBotManager::AllocateNewPlayer(edict_t* edict)
{
	// Creates a new player, registration is done at the constructor
	return new CPluginPlayer(edict);
}

void CPluginBotManager::OnClientPutInServer(edict_t* edict)
{
	AllocateNewPlayer(edict);
}

void CPluginBotManager::RegisterBot(IPluginBot* bot)
{
	if (std::find(m_botlist.begin(), m_botlist.end(), bot) == m_botlist.end() )
	{
		m_botlist.push_back(bot);
		LOG_CONSOLE(PLID, "Bot update rate is set to %i", TIME_TO_TICKS(CVAR_GET_FLOAT("gb_update_rate")));
	}
	else
	{
		LOG_CONSOLE(PLID, "Warning: Register called for already registered plugin bot!");
		delete bot;
	}
}

void CPluginBotManager::RegisterPlayer(IPlayer* player)
{
	if (std::find(m_playerlist.begin(), m_playerlist.end(), player) == m_playerlist.end())
	{
		LOG_CONSOLE(PLID, "New plugin player registered %i", ENTINDEX(player->GetEdict()));
		m_playerlist.push_back(player);
	}
	else
	{
		LOG_CONSOLE(PLID, "Warning: Register called for already registered player!");
		delete player;
	}
}

void CPluginBotManager::UnregisterBot(IPluginBot* bot)
{
	auto position = std::find(m_botlist.begin(), m_botlist.end(), bot);

	if (position != m_botlist.end())
	{
		m_botlist.erase(position);
	}
}

void CPluginBotManager::UnregisterPlayer(IPlayer* player)
{
	auto position = std::find(m_playerlist.begin(), m_playerlist.end(), player);

	if (position != m_playerlist.end())
	{
		m_playerlist.erase(position);
	}
}

bool CPluginBotManager::ClientCommand(IPlayer* player, const char* command, const std::vector<const char*>& args, const int argc)
{
	auto isdebug = CVAR_GET_FLOAT("gb_debug_enabled");

	if (isdebug >= 1.0f)
	{
		if (strcmp(command, "mathlibtest") == 0)
		{
			Vector position = player->GetPosition();
			Vector vangles(player->GetEdict()->v.v_angle);
			Vector mangles(player->GetEdict()->v.angles);
			Vector forward, right, up;
			AngleVectors(vangles, &forward, &right, &up);
			forward.NormalizeInPlace();
			LOG_CONSOLE(PLID, "Position <%3.2f, %3.2f, %3.2f> \n View Angles <%3.2f, %3.2f %3.2f> \n Angles: <%3.2f, %3.2f, %3.2f> \n Forward: <%3.2f, %3.2f, %3.2f>",
				position[0], position[1], position[2], vangles[0], vangles[1], vangles[2], mangles[0], mangles[1], mangles[2], forward[0], forward[1], forward[2]);

			MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, nullptr, player->GetEdict());
			WRITE_BYTE(TE_BEAMPOINTS);
			WRITE_COORD(position[0]);
			WRITE_COORD(position[1]);
			WRITE_COORD(position[2]);
			WRITE_COORD(position[0] + (forward[0] * 256.0f));
			WRITE_COORD(position[1] + (forward[1] * 256.0f));
			WRITE_COORD(position[2] + (forward[2] * 256.0f));
			WRITE_SHORT(TheWaypoints->GetSpriteTexture());
			WRITE_BYTE(1); // framestart
			WRITE_BYTE(10); // framerate
			WRITE_BYTE(200); // life in 0.1's
			WRITE_BYTE(30); // width
			WRITE_BYTE(0);  // noise
			WRITE_BYTE(0);   // r, g, b
			WRITE_BYTE(255);   // r, g, b
			WRITE_BYTE(0);   // r, g, b
			WRITE_BYTE(230);   // brightness
			WRITE_BYTE(5);    // speed
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, nullptr, player->GetEdict());
			WRITE_BYTE(TE_BEAMPOINTS);
			WRITE_COORD(position[0]);
			WRITE_COORD(position[1]);
			WRITE_COORD(position[2]);
			WRITE_COORD(position[0] + (right[0] * 256.0f));
			WRITE_COORD(position[1] + (right[1] * 256.0f));
			WRITE_COORD(position[2] + (right[2] * 256.0f));
			WRITE_SHORT(TheWaypoints->GetSpriteTexture());
			WRITE_BYTE(1); // framestart
			WRITE_BYTE(10); // framerate
			WRITE_BYTE(200); // life in 0.1's
			WRITE_BYTE(30); // width
			WRITE_BYTE(0);  // noise
			WRITE_BYTE(0);   // r, g, b
			WRITE_BYTE(0);   // r, g, b
			WRITE_BYTE(255);   // r, g, b
			WRITE_BYTE(230);   // brightness
			WRITE_BYTE(5);    // speed
			MESSAGE_END();

			return true;
		}
		else if (strcmp(command, "collectortest") == 0)
		{
			Vector position = player->GetPosition();
			Vector delta(512.0f, 512.0f, 512.0f);
			std::vector<edict_t*> list;

			UTIL_CollectEntitiesInABox(list, position - delta, position + delta);

			auto count = list.size();

			LOG_CONSOLE(PLID, "Collected %i entities!", count);

			for (auto y : list)
			{
				auto cls = STRING(y->v.classname);
				LOG_CONSOLE(PLID, "Found %s", cls);
			}

			return true;
		}
		else if (strcmp(command, "lookatme") == 0)
		{
			for (auto bot : m_botlist)
			{
				bot->GetBodyInterface()->AimHeadTowards(player->GetEdict(), IBody::CRITICAL, 10.0f, nullptr);
			}

			ClientPrint(VARS(player->GetEdict()), HUD_PRINTTALK, "Bots are now looking at you!");

			return true;
		}
		else if (strcmp(command, "dumpallentities") == 0)
		{
			for (int i = 1; i < gpGlobals->maxEntities; i++)
			{
				edict_t* edict = INDEXENT(i);

				if (!edict)
				{
					LOG_CONSOLE(PLID, "Entity %i is nullptr!", i);
					continue;
				}

				if (FNullEnt(edict))
				{
					LOG_CONSOLE(PLID, "Entity %i FNullEnt returns true!", i);
					continue;
				}

				if (edict->free)
				{
					LOG_CONSOLE(PLID, "Entity %i is free!", i);
					continue;
				}

				if (edict->pvPrivateData == nullptr)
				{
					LOG_CONSOLE(PLID, "Entity %i(%i) private data is nullptr!", i, edict->serialnumber);
					continue;
				}

				auto cls = STRING(edict->v.classname);

				LOG_CONSOLE(PLID, "Found entity #%i(%i)<%s>", i, edict->serialnumber, cls);
			}


			return true;
		}
		else if (strcmp(command, "findapath") == 0)
		{
			static Vector start(0.0f, 0.0f, 0.0f);
			static Vector end(0.0f, 0.0f, 0.0f);
			static bool hasstart = false;

			if (!hasstart)
			{
				hasstart = true;
				start = player->GetPosition();
				ClientPrint(VARS(player->GetEdict()), HUD_PRINTTALK, "Start position set! \n");
			}
			else
			{
				end = player->GetPosition();
				hasstart = false;
				CWaypoint* wptstart = TheWaypoints->GetNearestWaypoint(start);
				CWaypoint* wptend = TheWaypoints->GetNearestWaypoint(end);
				IPathCostFunctor defaultcost;
				CSimpleAStarSearch<IPathCostFunctor> search;

				bool pfresult = search.BuildPath(wptstart, wptend, &end, defaultcost);

				if (!pfresult)
				{
					ClientPrint(VARS(player->GetEdict()), HUD_PRINTTALK, "Build path failed! \n");
				}
				else
				{
					ClientPrint(VARS(player->GetEdict()), HUD_PRINTTALK, "Path built! \n");
					auto& path = search.GetPath();

					Vector p1 = start;
					Vector p2;
					bool first = true;

					for (auto y : path)
					{
						p2 = y->GetCenter();

						if (first) {
							UTIL_DrawBeamPoints(p1, p2, 240, 240, 0, 0);
							first = false;
						}
						else { UTIL_DrawBeamPoints(p1, p2, 240, 0, 60, 240); }
						
						p1 = y->GetCenter();
					}

					p2 = end;
					UTIL_DrawBeamPoints(p1, p2, 240, 0, 250, 0);
				}
			}

			return true;
		}
		else if (strcmp(command, "movetome") == 0)
		{
			for (auto bot : m_botlist)
			{
				bot->GetLocomotionInterface()->Run();
				bot->GetLocomotionInterface()->Approach(player->GetPosition());
			}

			ClientPrint(VARS(player->GetEdict()), HUD_PRINTTALK, "Moving to you! \n");

			return true;
		}
		else if (strcmp(command, "pvdatatest") == 0)
		{
			CBaseEntity* baseentity = static_cast<CBaseEntity*>(player->GetEdict()->pvPrivateData);

			if (baseentity)
			{
				LOG_MESSAGE(PLID, "9mm Ammo: %i", baseentity->ammo_9mm);
			}
			else
			{
				LOG_MESSAGE(PLID, "Failed to cast edict private data to CBaseEntity!");
			}

			return true;
		}
	}

	return false;
}

IPlayer* CPluginBotManager::GetPlayerOfEdict(edict_t* edict)
{
	int edictIndex = ENTINDEX(edict);

	for (auto player : m_playerlist)
	{
		if (ENTINDEX(player->GetEdict()) == edictIndex)
		{
			return player;
		}
	}

	return nullptr;
}

IPluginBot* CPluginBotManager::GetBotOfEdict(edict_t* edict)
{
	int edictIndex = ENTINDEX(edict);

	for (auto bot : m_botlist)
	{
		if (ENTINDEX(bot->GetEdict()) == edictIndex)
		{
			return bot;
		}
	}

	return nullptr;
}
