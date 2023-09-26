// vi: set ts=4 sw=4 :
// vim: set tw=75 :

/*
 * Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <memory>


#include <extdll.h>
#include <dllapi.h>
#include <meta_api.h>
#include <cstring>

#include "plugincvars.h"
#include "pluginglobals.h"
#include "pluginplayer.h"
#include "pluginutil.h"
#include "interfaces/pluginbot.h"
#include "console_commands.h"
#include "mods/mod_base.h"
#include "manager.h"
#include "waypoints/waypoint_manager.h"
#include "sdk/chandle.h"

void SV_GameInit(void)
{
	std::unique_ptr<char[]> gamedir(new char[256]);
	GET_GAME_DIR(gamedir.get());
	Mods::IdentifyGame(gamedir.get());

	if (gamemod == nullptr)
	{
		gamemod = GameModFactory();
	}

	if (TheBotManager == nullptr)
	{
		TheBotManager = new CPluginBotManager;
	}

	if (TheWaypoints == nullptr)
	{
		TheWaypoints = WaypointManagerFactory();
	}

	auto cvar = CVAR_GET_POINTER("sys_ticrate");
	auto fps = CVAR_GET_POINTER("fps_max");

	if (cvar && fps)
	{
		if (cvar->value <= fps->value)
		{
			g_flTickInterval = 1.0f / cvar->value;
		}
		else
		{
			g_flTickInterval = 1.0f / fps->value;
		}
	}

	CVAR_REGISTER(&gb_update_rate);
	CVAR_REGISTER(&gb_debug_enabled);
	CVAR_REGISTER(&gb_nav_zdraw);
	CVAR_REGISTER(&gb_nav_quicksave);

	RegisterServerCommands();
	RegisterWaypointEditingCommands();

	TheBotManager->CreatePluginDirectories(gamedir.get());

	RETURN_META(MRES_IGNORED);
}

int SV_Spawn(edict_t* pent)
{
	auto classname = STRING(pent->v.classname);

	if (strcmp(classname, "worldspawn") == 0)
	{
		int spr = PRECACHE_MODEL("sprites/lgtning.spr");
		PRECACHE_SOUND("buttons/blip1.wav");
		PRECACHE_SOUND("buttons/button3.wav");
		TheWaypoints->SetSpriteTexture(spr);
		UTIL_SetBeamSpriteModel(spr);
		TheWaypoints->OnMapChange();
		gamemod->OnRoundRestart();
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void SV_StartFrame(void)
{
	TheBotManager->Update();
	TheWaypoints->EditingUpdate();
	RETURN_META(MRES_IGNORED);
}

void OnClientPutInServer(edict_t* pEntity)
{
	TheBotManager->OnClientPutInServer(pEntity);
	gamemod->OnClientPutInServer(pEntity);
	RETURN_META(MRES_IGNORED);
}

void OnClientDisconnect(edict_t* pEntity)
{
	gamemod->OnClientDisconnect(pEntity);
	IPlayer* player = TheBotManager->GetPlayerOfEdict(pEntity);
	IPluginBot* bot = TheBotManager->GetBotOfEdict(pEntity);

	if (player)
	{
		delete player;
	}

	if (bot)
	{
		delete bot;
	}

	RETURN_META(MRES_IGNORED);
}

void OnClientKill(edict_t* pEntity)
{
	RETURN_META(MRES_IGNORED);
}

void OnClientCommand(edict_t* pEntity)
{
	bool iscommand = false;
	auto player = TheBotManager->GetPlayerOfEdict(pEntity);
	int args = CMD_ARGC(); // actual argument count is this - 1
	const char* szcommand = CMD_ARGV(0);
	std::vector<const char*> arglist;

	for (int i = 1; i < args; i++)
	{
		auto szarg = CMD_ARGV(i);
		arglist.push_back(szarg);
	}

	if (player != nullptr)
	{
		iscommand = TheBotManager->ClientCommand(player, szcommand, arglist, args - 1);

		if (!iscommand)
		{
			iscommand = TheWaypoints->ClientCommand(player, szcommand, arglist, args - 1);
		}
	}

	if (iscommand) { RETURN_META(MRES_SUPERCEDE); }
	RETURN_META(MRES_IGNORED);
}

static DLL_FUNCTIONS gFunctionTable = 
{
	SV_GameInit,					// pfnGameInit
	SV_Spawn,					// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NULL,					// pfnTouch
	NULL,					// pfnBlocked
	NULL,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NULL,					// pfnSetAbsBox

	NULL,					// pfnSaveWriteFields
	NULL,					// pfnSaveReadFields

	NULL,					// pfnSaveGlobalState
	NULL,					// pfnRestoreGlobalState
	NULL,					// pfnResetGlobalState

	NULL,					// pfnClientConnect
	OnClientDisconnect,					// pfnClientDisconnect
	OnClientKill,					// pfnClientKill
	OnClientPutInServer,					// pfnClientPutInServer
	OnClientCommand,					// pfnClientCommand
	NULL,					// pfnClientUserInfoChanged
	NULL,					// pfnServerActivate
	NULL,					// pfnServerDeactivate

	NULL,					// pfnPlayerPreThink
	NULL,					// pfnPlayerPostThink

	SV_StartFrame,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink
	
	NULL,					// pfnSys_Error

	NULL,					// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType
	
	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NULL,					// pfnAddToFullPack
	NULL,					// pfnCreateBaseline
	NULL,					// pfnRegisterEncoders
	NULL,					// pfnGetWeaponData
	NULL,					// pfnCmdStart
	NULL,					// pfnCmdEnd
	NULL,					// pfnConnectionlessPacket
	NULL,					// pfnGetHullBounds
	NULL,					// pfnCreateInstancedBaselines
	NULL,					// pfnInconsistentFile
	NULL,					// pfnAllowLagCompensation
};

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, 
		int *interfaceVersion)
{
	if(!pFunctionTable) {
		UTIL_LogPrintf("GetEntityAPI2 called with null pFunctionTable");
		return(FALSE);
	}
	else if(*interfaceVersion != INTERFACE_VERSION) {
		UTIL_LogPrintf("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));
	return(TRUE);
}
