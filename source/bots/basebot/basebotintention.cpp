#include <extdll.h>
#include <meta_api.h>
#include "interfaces/pluginbot.h"
#include "bots/basebot.h"
#include "basebotintention.h"


CBaseBotIntention::CBaseBotIntention(IPluginBot* bot) : IIntention(bot)
{
	m_behavior = new IBehavior<CBaseBot>(new CBaseBotMainAction, "BaseBotBehavior");
}

CBaseBotIntention::~CBaseBotIntention()
{
	delete m_behavior;
}

void CBaseBotIntention::Reset()
{
	if (m_behavior)
	{
		delete m_behavior;
	}

	m_behavior = new IBehavior<CBaseBot>(new CBaseBotMainAction, "BaseBotBehavior");

	IIntention::Reset();
}

void CBaseBotIntention::Update()
{
	m_behavior->Update(static_cast<CBaseBot*>(GetBot()), GetUpdateInterval());
}

void CBaseBotIntention::UpKeep()
{
	IIntention::UpKeep();
}
