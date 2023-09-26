
#include <extdll.h>
#include <meta_api.h>
#include "manager.h"
#include "interfaces/player.h"
#include "interfaces/pluginbot.h"
#include "bots/basebot.h"
#include "mod_base.h"
#include "mod_hl1.h"

Mods::ID g_currentMod;

namespace Mods
{
	void IdentifyGame(const char* gamefolder)
	{
		g_currentMod = Game_Unknown;
		LOG_CONSOLE(PLID, "Detected game directory: %s", gamefolder);

		if (strcmp(gamefolder, "valve") == 0)
		{
			g_currentMod = Game_HL1;
		}
		else if (strcmp(gamefolder, "gearbox") == 0)
		{
			g_currentMod = Game_OP4;
		}
		else if (strcmp(gamefolder, "svencoop") == 0)
		{
			g_currentMod = Game_Svencoop;
		}
	}
}

CBaseMod::CBaseMod()
{
}

CBaseMod::~CBaseMod()
{
}

int CBaseMod::GetClientCount(bool includeBots)
{
	int count = 0;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		auto client = INDEXENT(i);

		if (client && client->pvPrivateData != nullptr)
		{
			if (includeBots && client->v.flags & FL_FAKECLIENT)
			{
				count++; // count bots
				continue;
			}

			if (!includeBots && client->v.flags & FL_FAKECLIENT)
				continue; // skip bots

			count++; // not flagged as fake client
		}
	}

	return count;
}

IPluginBot* CBaseMod::CreateAndAddBot()
{
	edict_t* edict = nullptr;
	CBaseBot* bot = nullptr;
	int index = 0;

	if (GetClientCount(true) >= gpGlobals->maxClients)
	{
		LOG_CONSOLE(PLID, "Failed to create bot! Server is full!");
		return nullptr;
	}

	edict = g_engfuncs.pfnCreateFakeClient("Goldbot");

	if (!edict)
	{
		LOG_CONSOLE(PLID, "Engine failed to create a fake client!");
		return nullptr;
	}

	// create the player entity by calling MOD's player function
	// (from LINK_ENTITY_TO_CLASS for player object)
	CALL_GAME_ENTITY(PLID, GetPlayerEntityClassname(), VARS(edict));

	auto infobuffer = GET_INFOKEYBUFFER(edict);
	index = ENTINDEX(edict);

	SET_CLIENT_KEYVALUE(index, infobuffer, "model", ""); // to-do: implement model selection for mods that have it
	SET_CLIENT_KEYVALUE(index, infobuffer, "rate", "3500.000000");
	SET_CLIENT_KEYVALUE(index, infobuffer, "cl_updaterate", "20");
	// Thanks Immortal_BLG for finding that cl_lw and cl_lc need to be 0 to fix bots getting stuck inside each other
	SET_CLIENT_KEYVALUE(index, infobuffer, "cl_lw", "0");
	SET_CLIENT_KEYVALUE(index, infobuffer, "cl_lc", "0");
	SET_CLIENT_KEYVALUE(index, infobuffer, "_vgui_menus", "0");

	char reason[128]{};
	MDLL_ClientConnect(edict, "Goldbot", "127.0.0.1", reason);
	MDLL_ClientPutInServer(edict);

	edict->v.flags |= FL_FAKECLIENT;

	bot = AllocateBot(edict);
	LOG_CONSOLE(PLID, "Successfully created and allocated a new bot!");
	bot->GetVisionInterface();
	
	return bot;
}

CBaseBot* CBaseMod::AllocateBot(edict_t* edict)
{
	return new CBaseBot(edict);
}

CBaseMod* GameModFactory()
{
	switch (g_currentMod)
	{
	case Mods::Game_HL1:
		return new CHL1Mod;
	case Mods::Game_Unknown:
	case Mods::Game_OP4:
	case Mods::Game_Svencoop:
	case Mods::Game_MAX:
	default:
		return new CBaseMod;
		break;
	}
}

CBaseMod* gamemod = nullptr;