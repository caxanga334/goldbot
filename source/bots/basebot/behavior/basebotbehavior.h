#ifndef BASE_BOT_BEHAVIOR_H_
#define BASE_BOT_BEHAVIOR_H_

#include "interfaces/behavior.h"

class CBaseBot;

class CBaseBotMainAction : public Action<CBaseBot>
{
public:
	virtual ActionResult<CBaseBot> OnStart(CBaseBot* me, Action<CBaseBot>* priorAction) override;
	virtual ActionResult<CBaseBot> Update(CBaseBot* me, float interval) override;
	virtual EventDesiredResult<CBaseBot> OnSight(CBaseBot* me, CMemoryEntity* them) override;

	virtual const char* GetName(void) const override { return "BaseBotMainAction"; }
};

#endif // !BASE_BOT_BEHAVIOR_H_
