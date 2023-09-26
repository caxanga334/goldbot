#include <extdll.h>
#include "plugincvars.h"

cvar_t gb_update_rate = { "gb_update_rate", "0.06", FCVAR_SERVER | FCVAR_EXTDLL };
cvar_t gb_debug_enabled = { "gb_debug_enabled", "0", FCVAR_SERVER | FCVAR_EXTDLL };
cvar_t gb_nav_zdraw = { "gb_nav_zdraw", "1", FCVAR_SERVER | FCVAR_EXTDLL };
cvar_t gb_nav_quicksave = { "gb_nav_quicksave", "0", FCVAR_SERVER | FCVAR_EXTDLL };