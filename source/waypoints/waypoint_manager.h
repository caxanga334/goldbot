#ifndef WAYPOINT_MANAGER_H_
#define WAYPOINT_MANAGER_H_

#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>

#include "sdk/chandle.h"

constexpr auto WAYPOINT_FILE_HEADER = "GOLDBOT";

class IPlayer;
class CWaypoint;
class CPluginPlayer;
class Vector;

class CWaypointManager
{
public:
	CWaypointManager();
	virtual ~CWaypointManager();

	// Called every frame when waypoint editing is enabled
	virtual void EditingUpdate();

	virtual void OnMapChange();

	virtual bool ClientCommand(IPlayer* player, const char* command, const std::vector<const char*>& args, const int argc);

	// Allocates a new waypoint object
	virtual CWaypoint* WaypointFactory();
	// Creates and stores a new waypoint
	virtual CWaypoint* CreateWaypoint(IPlayer*player);

	bool DeleteWaypoint(CWaypoint* todelete);

	// Waypoint core version, non virtual because it's the same for all mods
	int GetVersion() { return 0; }
	// Waypoint sub-version, mods can override these
	virtual int GetSubversion() { return 0; }

	void SetSpriteTexture(const int texture) { m_spriteTexture = texture; }
	int GetSpriteTexture() { return m_spriteTexture; }

	std::vector<CWaypoint*>& GetTheWaypoints() { return m_waypoints; }

	// Base Save/Load, not virtual because there shouldn't be overriden

	void Save();
	bool Load();

	// Mod specific custom save data, before waypoint data

	virtual void PreWaypointCustomSave(std::fstream& file, const int subVersion) {}
	virtual bool PreWaypointCustomLoad(std::fstream& file, const int subVersion) { return true; }

	// Mod specific custom save data, after waypoint data

	virtual void PostWaypointCustomSave(std::fstream& file, const int subVersion) {}
	virtual bool PostWaypointCustomLoad(std::fstream& file, const int subVersion) { return true; }

	// Gets the current waypoint editor, nullptr if no editor
	edict_t* GetEditor() { return m_editor.Get(); }
	void SetEditor(edict_t* edict) { m_editor = edict; }

	bool IsLoaded() { return m_loaded; }
	bool IsEditing() { return m_editmode; }
	void ToggleEditing();

	CWaypoint* GetWaypointOfID(const int id);
	CWaypoint* GetNearestWaypoint(const Vector& position);
	// Maximum distance is squared
	CWaypoint* GetNearestWaypoint(const Vector& position, const float maxdistsqr);

	/**
	 * @brief Collects waypoints in a radius
	 * @param source Position
	 * @param radius Squared radius
	 * @param waypointvector Vector to store the waypoints at
	 * @return Number of waypoints collected
	*/
	int CollectWaypointsInRadius(const Vector& source, const float radius, std::vector<CWaypoint*>& waypointvector);

protected:
	int m_spriteTexture;
	float m_editupdatetimer;
	float m_authpathdist;

private:
	bool m_editmode; // Controls editing mode
	bool m_loaded;
	int m_pathstart; // When creating a new path between waypoints, this holds the ID of the starting waypoint
	std::vector<CWaypoint*> m_waypoints; // Vector where all waypoints are stored
	CHandle m_editor; // waypoint editing player
};

// Waypoint manager singleton
extern CWaypointManager* TheWaypoints;
extern CWaypointManager* WaypointManagerFactory();

void RegisterWaypointEditingCommands();

#endif // !WAYPOINT_MANAGER_H_

