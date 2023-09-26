#ifndef BASE_BOT_INTENTION_INTERFACE_H_
#define BASE_BOT_INTENTION_INTERFACE_H_

#include "interfaces/intention.h"
#include "interfaces/behavior.h"
#include "behavior/basebotbehavior.h"

class CBaseBot;

class CBaseBotIntention : public IIntention
{
public:
	CBaseBotIntention(IPluginBot* bot);
	virtual ~CBaseBotIntention();

	virtual void Reset() override;
	virtual void Update() override;
	virtual void UpKeep() override;
	virtual float GetUpdateInterval() override;

	virtual IEventResponder* FirstContainedResponder(void) const override { return m_behavior; }
	virtual IEventResponder* NextContainedResponder(IEventResponder* current) const override { return nullptr; }

private:
	IBehavior<CBaseBot>* m_behavior;
};

inline float CBaseBotIntention::GetUpdateInterval()
{
	return 0.07f;
}

#endif // !BASE_BOT_INTENTION_INTERFACE_H_
