
#include <extdll.h>
#include <meta_api.h>

#include "mods/mod_base.h"
#include "pluginglobals.h"
#include "pluginutil.h"
#include "waypoints/waypoint_manager.h"


#include <string>
#include <fstream>
#include <filesystem>

static void ConCommand_ModInfo()
{
	auto modname = gamemod->GetModName();
	auto modid = static_cast<int>(g_currentMod);
	LOG_CONSOLE(PLID, "Detected mod: %s (%i)", modname, modid);
}

static void ConCommand_AddBot()
{
	gamemod->CreateAndAddBot();
}

static void ConCommand_EngineInfo()
{
	auto map = STRING(gpGlobals->mapname);
	auto clients = gpGlobals->maxClients;
	auto entities = gpGlobals->maxEntities;
	auto time = gpGlobals->time;

	LOG_CONSOLE(PLID, "Engine info:\nMap: %s\nClients %i Entities %i\nTime: %.4f", map, clients, entities, time);
}

static void ConCommand_CreateDirTest()
{
	TheWaypoints->Save();
}

void RegisterServerCommands()
{
	g_engfuncs.pfnAddServerCommand("gb_modinfo", ConCommand_ModInfo);
	g_engfuncs.pfnAddServerCommand("gb_addbot", ConCommand_AddBot);
	g_engfuncs.pfnAddServerCommand("gb_engineinfo", ConCommand_EngineInfo);
	g_engfuncs.pfnAddServerCommand("gb_createdirtest", ConCommand_CreateDirTest);
}