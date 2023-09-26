#include <extdll.h>
#include <meta_api.h>
#include "basebot/basebotintention.h"
#include "basebot.h"

CBaseBot::CBaseBot(edict_t* edict) : IPluginBot(edict)
{
	m_intention = new CBaseBotIntention(this);
	m_locomotion = new CBaseHalfLifeLocomotion(this);
}

CBaseBot::~CBaseBot()
{
	delete m_intention;
	delete m_locomotion;
}
