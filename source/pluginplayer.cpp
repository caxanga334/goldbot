#include <extdll.h>
#include <meta_api.h>
#include "interfaces/pluginbot.h"
#include "pluginplayer.h"
#include "manager.h"

CPluginPlayer::CPluginPlayer(edict_t* edict) : IPlayer(edict)
{
	TheBotManager->RegisterPlayer(this);
}

CPluginPlayer::~CPluginPlayer()
{
	TheBotManager->UnregisterPlayer(this);
}
