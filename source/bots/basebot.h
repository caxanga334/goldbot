#ifndef BASE_BOT_H_
#define BASE_BOT_H_

#include "interfaces/pluginbot.h"
#include "interfaces/basehllocomotion.h"
#include "basebot/basebotintention.h"

class CBaseBot : public IPluginBot
{
public: 
	CBaseBot(edict_t* edict);
	virtual ~CBaseBot();

	virtual CBaseBotIntention* GetIntentionInterface() const override { return m_intention; }
	virtual CBaseHalfLifeLocomotion* GetLocomotionInterface() const override { return m_locomotion; }

private:
	CBaseBotIntention* m_intention;
	CBaseHalfLifeLocomotion* m_locomotion;
};

#endif // !BASE_BOT_H_

