#include <extdll.h>
#include <meta_api.h>
#include "mods/mod_base.h"
#include "waypoint_manager.h"

/**
 * @brief Allocates the waypoint manager, allows the creation of mod specific waypoint managers
*/
CWaypointManager* WaypointManagerFactory()
{
	return new CWaypointManager;
}