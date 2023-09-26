#include <extdll.h>
#include <meta_api.h>
#include <cstring>
#include "manager.h"
#include "pluginplayer.h"
#include "mods/mod_base.h"
#include "interfaces/pluginbot.h"
#include "bots/basebot.h"
#include "waypoint_base.h"
#include "waypoint_manager.h"

static void ConCommand_WptToggleEditing()
{
	TheWaypoints->ToggleEditing();
}

void RegisterWaypointEditingCommands()
{
	g_engfuncs.pfnAddServerCommand("gb_toggle_waypoint_edit", ConCommand_WptToggleEditing);
}