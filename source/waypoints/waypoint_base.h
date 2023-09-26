#ifndef WAYPOINT_BASE_H_
#define WAYPOINT_BASE_H_

#include "sdk/math_vectors.h"
#include "sdk/color.h"
#include <cstring>

class IPlayer;
class CPluginPlayer;

namespace WaypointConst
{
	constexpr float WaypointHeight = 72.0f;
	constexpr float WaypointCenter = 72.0f / 2.0f;
	constexpr float WaypointDrawDistance = 512.0f * 512.0f; // Squared distance for waypoint drawing
	constexpr int MaxPaths = 64;
	constexpr int InvalidPathConnection = -1;
	constexpr float AuthPathDistance = 400.0f * 400.0f;
}

namespace WaypointPath
{
	enum PathType
	{
		PATH_NORMAL = 0, // Normal path type, walk straigh from point A to point B
		PATH_JUMP, // Requires jumping over an obstacle
		PATH_GAPJUMP, // Requires jumping over a gap on the floor
		PATH_RAMPSTAIRS, // Walk straigh for stairs and ramps
		PATH_LADDER, // Ladder movement

		MAX_PATH_TYPES
	};

	inline PathType NameToType(const char* name)
	{
		if (strcmp(name, "normal") == 0)
		{
			return PATH_NORMAL;
		}
		else if (strcmp(name, "jump") == 0)
		{
			return PATH_JUMP;
		}
		else if (strcmp(name, "gapjump") == 0)
		{
			return PATH_GAPJUMP;
		}
		else if (strcmp(name, "gap") == 0)
		{
			return PATH_GAPJUMP;
		}
		else if (strcmp(name, "stairs") == 0)
		{
			return PATH_RAMPSTAIRS;
		}
		else if (strcmp(name, "ramp") == 0)
		{
			return PATH_RAMPSTAIRS;
		}
		else if (strcmp(name, "ladder") == 0)
		{
			return PATH_LADDER;
		}
		else
		{
			return PATH_NORMAL;
		}
	}

	inline const char* TypeToString(PathType type)
	{
		switch (type)
		{
		case WaypointPath::PATH_NORMAL:
			return "NORMAL";
		case WaypointPath::PATH_JUMP:
			return "JUMP";
		case WaypointPath::PATH_GAPJUMP:
			return "GAP JUMP";
		case WaypointPath::PATH_RAMPSTAIRS:
			return "RAMP/STAIRS";
		case WaypointPath::PATH_LADDER:
			return "LADDER";
		case WaypointPath::MAX_PATH_TYPES:
		default:
			return "UNKNOWN";
		}
	}
}

class CWaypoint
{
public:
	CWaypoint();
	virtual ~CWaypoint();

	virtual void Init(const int id, const Vector& position, const float yaw);

	bool operator==(const CWaypoint& other) const { return this->m_id == other.m_id; }
	bool operator!=(const CWaypoint& other) const { return this->m_id != other.m_id; }

	int GetID() const { return m_id; }

	void Save(std::fstream& file, const int version);
	bool Load(std::fstream& file, const int version);

	virtual void SaveCustom(std::fstream& file, const int subVersion) {}
	virtual bool LoadCustom(std::fstream& file, const int subVersion) { return true; }

	virtual void Draw();
	virtual void DrawPath();

	void PostAllWaypointsLoaded();

	// Squared distance to position vector
	float DistToSqr(const Vector& source) const;
	float DistTo(const Vector& source) const;
	float DistToSqr(const CWaypoint* other) const;
	float DistTo(const CWaypoint* other) const;

	// Notifies this waypoint that a waypoint of the given ID was deleted
	virtual void NotifyWaypointDeletion(const int id);
	virtual void PrintWaypointInfo(IPlayer* player);

	bool AddPathTo(CWaypoint* other);
	bool SetPathTypeTo(CWaypoint* other, WaypointPath::PathType type);
	bool DeletePathTo(CWaypoint* other);
	void AutoPathTo(CWaypoint* other);
	const Vector& GetPosition() const { return m_position; }
	Vector GetCenter();

	float GetZ() const { return m_position.z; }

	int* GetPaths() { return m_paths; }

	// Returns true if this waypoint has a path connection to the other waypoint
	bool HasPathTo(CWaypoint* other) const;
	bool HasPathTo(const int id) const;
	// Returns the path type to the other waypoint. Returns MAX_PATH_TYPES if not connected to the other waypoint
	WaypointPath::PathType GetPathTypeTo(CWaypoint* other) const;
	WaypointPath::PathType GetPathTypeTo(const int id) const;
	
	/**
	 * @brief Gets a waypoint pointer at the given path index
	 * @param index Path connection array index
	 * @return Waypoint pointer or nullptr if no connection at the given index
	*/
	CWaypoint* GetWaypointOfPath(const int index) const;

protected:
	void InternalDraw(Color& color);
	void InternalDraw(Color& color, Vector& start, Vector& end);

private:
	int m_id; // waypoint ID
	Vector m_position; // waypoint world position
	float m_yaw; // waypoint direction (yaw)
	int m_paths[WaypointConst::MaxPaths]; // Array of IDs of connected waypoints
	int m_pathstype[WaypointConst::MaxPaths]; // Type of path
	CWaypoint* m_connections[WaypointConst::MaxPaths]; // Pointers to connected waypoints
};

inline float CWaypoint::DistToSqr(const Vector& source) const
{
	return m_position.DistToSqr(source);
}

inline float CWaypoint::DistTo(const Vector& source) const
{
	return m_position.DistTo(source);
}

inline float CWaypoint::DistToSqr(const CWaypoint* other) const
{
	return m_position.DistToSqr(other->m_position);
}

inline float CWaypoint::DistTo(const CWaypoint* other) const
{
	return m_position.DistTo(other->m_position);
}

inline bool CWaypoint::HasPathTo(CWaypoint* other) const
{
	const int id = other->GetID();
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == id)
			return true;
	}

	return false;
}

inline bool CWaypoint::HasPathTo(const int id) const
{
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == id)
			return true;
	}

	return false;
}

inline WaypointPath::PathType CWaypoint::GetPathTypeTo(CWaypoint* other) const
{
	const int id = other->GetID();
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == id)
			return static_cast<WaypointPath::PathType>(m_pathstype[i]);
	}

	return WaypointPath::MAX_PATH_TYPES;
}

inline WaypointPath::PathType CWaypoint::GetPathTypeTo(const int id) const
{
	for (int i = 0; i < WaypointConst::MaxPaths; i++)
	{
		if (m_paths[i] == id)
			return static_cast<WaypointPath::PathType>(m_pathstype[i]);
	}

	return WaypointPath::MAX_PATH_TYPES;
}

inline CWaypoint* CWaypoint::GetWaypointOfPath(const int index) const
{
	return m_connections[index];
}

#endif // !WAYPOINT_BASE_H_
