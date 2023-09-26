#include <extdll.h>
#include <meta_api.h>

#undef open
#undef read
#undef write
#undef close

#include <fstream>
#include <filesystem>

#include "manager.h"
#include "mods/mod_base.h"
#include "pluginplayer.h"
#include "interfaces/player.h"
#include "interfaces/pluginbot.h"
#include "bots/basebot.h"
#include "waypoint_manager.h"
#include "waypoint_base.h"

static Color s_normalcolor(0, 230, 0, 240); // Normal waypoint color
static Color s_pathcolor(255, 255, 255, 220); // Waypoint path color
static Color s_jumpcolor(255, 0, 0, 220); // Jump path color
static Color s_gapcolor(202, 21, 123, 220); // Gap jump path color
static Color s_anglepathcolor(255, 165, 0, 220); // Ramp/Stairs path color
static Color s_ladderpathcolor(127, 0, 127, 220);

// Same as MESSAGE_BEGIN but doesn't cause duplication errors
static void startMessage(int dest, int type, const float* origin, edict_t* ed)
{
	g_engfuncs.pfnMessageBegin(dest, type, origin, ed);
}

CWaypoint::CWaypoint() : m_position()
{
	m_id = -1;
	m_yaw = 0.0f;

	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		m_paths[i] = WaypointConst::InvalidPathConnection;
		m_pathstype[i] = WaypointPath::PATH_NORMAL; // default to normal
		m_connections[i] = nullptr;
	}
}

CWaypoint::~CWaypoint()
{
}

void CWaypoint::Init(const int id, const Vector& position, const float yaw)
{
	m_id = id;
	m_position = position;
	m_yaw = yaw;
}

void CWaypoint::Save(std::fstream& file, const int version)
{
	file.write(reinterpret_cast<char*>(&m_id), sizeof(int));
	file.write(reinterpret_cast<char*>(&m_position), sizeof(Vector));
	file.write(reinterpret_cast<char*>(&m_yaw), sizeof(float));
	file.write(reinterpret_cast<char*>(&m_paths), static_cast<std::streamsize>(WaypointConst::MaxPaths) * sizeof(int));
	file.write(reinterpret_cast<char*>(&m_pathstype), static_cast<std::streamsize>(WaypointConst::MaxPaths) * sizeof(int));
}

bool CWaypoint::Load(std::fstream& file, const int version)
{
	file.read(reinterpret_cast<char*>(&m_id), sizeof(int));
	file.read(reinterpret_cast<char*>(&m_position), sizeof(Vector));
	file.read(reinterpret_cast<char*>(&m_yaw), sizeof(float));
	file.read(reinterpret_cast<char*>(&m_paths), static_cast<std::streamsize>(WaypointConst::MaxPaths) * sizeof(int));
	file.read(reinterpret_cast<char*>(&m_pathstype), static_cast<std::streamsize>(WaypointConst::MaxPaths) * sizeof(int));

	if (file.good() == false)
	{
		LOG_CONSOLE(PLID, "Failed to load waypoint data from file!");
		return false;
	}

	return true;
}

void CWaypoint::Draw()
{
	InternalDraw(s_normalcolor);
}

void CWaypoint::DrawPath()
{
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] != WaypointConst::InvalidPathConnection)
		{
			CWaypoint* other = TheWaypoints->GetWaypointOfID(m_paths[i]);

			Vector start = GetCenter();
			Vector end = other->GetCenter();

			switch (m_pathstype[i])
			{
			case WaypointPath::PATH_JUMP:
				InternalDraw(s_jumpcolor, start, end);
				break;
			case WaypointPath::PATH_GAPJUMP:
				InternalDraw(s_gapcolor, start, end);
				break;
			case WaypointPath::PATH_RAMPSTAIRS:
				InternalDraw(s_anglepathcolor, start, end);
				break;
			case WaypointPath::PATH_LADDER:
				InternalDraw(s_ladderpathcolor, start, end);
				break;
			case WaypointPath::PATH_NORMAL:
			default:
				InternalDraw(s_pathcolor, start, end);
				break;
			}
		}
	}
}

void CWaypoint::PostAllWaypointsLoaded()
{
	for (int p = 0; p < WaypointConst::MaxPaths; p++)
	{
		m_connections[p] = TheWaypoints->GetWaypointOfID(m_paths[p]);
	}
}

void CWaypoint::NotifyWaypointDeletion(const int id)
{
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == id)
		{
			m_paths[i] = WaypointConst::InvalidPathConnection;
			m_pathstype[i] = WaypointPath::PATH_NORMAL;
			m_connections[i] = nullptr;
		}
	}
}

void CWaypoint::PrintWaypointInfo(IPlayer* player)
{
	auto vars = player->GetPev();
	char buffer[512]{};
	sprintf(buffer, "Waypoint #%i at <%3.2f, %3.2f, %3.2f> (%3.2f) \n", m_id, m_position[0], m_position[1], m_position[2], m_yaw);
	ClientPrint(vars, HUD_PRINTCONSOLE, buffer);

	int counter = 0;
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] != WaypointConst::InvalidPathConnection)
		{
			counter++;
		}
	}

	sprintf(buffer, "Waypoint is connected to %i other waypoints.", counter);
	ClientPrint(vars, HUD_PRINTCONSOLE, buffer);
}

bool CWaypoint::AddPathTo(CWaypoint* other)
{
	if (other == this)
		return false;

	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == other->GetID())
		{
			return true; // path already exists
		}
		else if (m_paths[i] == WaypointConst::InvalidPathConnection)
		{
			m_paths[i] = other->GetID();
			m_pathstype[i] = WaypointPath::PATH_NORMAL;
			m_connections[i] = other;
			return true;
		}
	}

	return false;
}

bool CWaypoint::SetPathTypeTo(CWaypoint* other, WaypointPath::PathType type)
{
	if (other == this)
		return false;

	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == other->GetID())
		{
			m_pathstype[i] = type;
			return true;
		}
	}

	return false;
}

bool CWaypoint::DeletePathTo(CWaypoint* other)
{
	if (other == this)
		return false;

	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == other->GetID())
		{
			m_paths[i] = WaypointConst::InvalidPathConnection;
			m_pathstype[i] = WaypointPath::PATH_NORMAL;
			m_connections[i] = nullptr;
		}
	}

	return true;
}

void CWaypoint::AutoPathTo(CWaypoint* other)
{
	if (other == this)
		return;

	// Important note about paths:
	// The bot locomotion interface will automatically filter connections

	Vector start = GetPosition();
	Vector end = other->GetPosition();
	float zdiff = fabsf(start.z - end.z);
	TraceResult tr;

	if (zdiff >= 18.0f) { return; } // TO-DO: Add support for mod specific step height
	
	UTIL_TraceLine(start, end, ignore_monsters, dont_ignore_glass, TheWaypoints->GetEditor(), &tr);

	if (tr.flFraction == 1.0f) // no obstruction between both waypoint origins
	{
		AddPathTo(other);
		return;
	}
}

Vector CWaypoint::GetCenter()
{
	Vector center(m_position);
	center[2] = center[2] + WaypointConst::WaypointCenter;
	return center;
}

void CWaypoint::InternalDraw(Color& color)
{
	startMessage(MSG_PVS, SVC_TEMPENTITY, m_position, nullptr);
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(m_position[0]);
	WRITE_COORD(m_position[1]);
	WRITE_COORD(m_position[2]);
	WRITE_COORD(m_position[0]);
	WRITE_COORD(m_position[1]);
	WRITE_COORD(m_position[2] + WaypointConst::WaypointHeight);
	WRITE_SHORT(TheWaypoints->GetSpriteTexture());
	WRITE_BYTE(1); // framestart
	WRITE_BYTE(10); // framerate
	WRITE_BYTE(10); // life in 0.1's
	WRITE_BYTE(30); // width
	WRITE_BYTE(0);  // noise

	WRITE_BYTE(color.GetRed());   // r, g, b
	WRITE_BYTE(color.GetGreen());   // r, g, b
	WRITE_BYTE(color.GetBlue());   // r, g, b

	WRITE_BYTE(color.GetAlpha());   // brightness
	WRITE_BYTE(5);    // speed
	MESSAGE_END();
}

void CWaypoint::InternalDraw(Color& color, Vector& start, Vector& end)
{
	startMessage(MSG_PVS, SVC_TEMPENTITY, start, nullptr);
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(start[0]);
	WRITE_COORD(start[1]);
	WRITE_COORD(start[2]);
	WRITE_COORD(end[0]);
	WRITE_COORD(end[1]);
	WRITE_COORD(end[2]);
	WRITE_SHORT(TheWaypoints->GetSpriteTexture());
	WRITE_BYTE(1); // framestart
	WRITE_BYTE(10); // framerate
	WRITE_BYTE(10); // life in 0.1's
	WRITE_BYTE(30); // width
	WRITE_BYTE(0);  // noise

	WRITE_BYTE(color.GetRed());   // r, g, b
	WRITE_BYTE(color.GetGreen());   // r, g, b
	WRITE_BYTE(color.GetBlue());   // r, g, b

	WRITE_BYTE(color.GetAlpha());   // brightness
	WRITE_BYTE(5);    // speed
	MESSAGE_END();
}

