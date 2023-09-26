#ifndef BODY_INTERFACE_H_
#define BODY_INTERFACE_H_

#include "sdk/timers.h"
#include "sdk/chandle.h"
#include "component.h"

class IBody : public IBotComponent
{
public:
	IBody(IPluginBot* bot);
	virtual ~IBody();

	enum LookPriority
	{
		BORING = 0, // Just looking around
		INTERESTING, // Last know position, a sound, etc
		IMPORTANT, // Something dangerous
		USE, // Looking at something thats needs to be used (wall chargers, buttons, usable-doors, pushables, etc)
		ENEMY, // Looking at an active threat
		CRITICAL, // Something of very high importance
		MOVEMENT, // Critical for high precision movement (ladders, jumps, etc)
		MANDATORY // Nothing can interrupt this
	};

	virtual void Reset() override;
	virtual void Update() override;
	virtual void UpKeep() override;

	void AimHeadTowards(const Vector& lookAtTarget, LookPriority priority = BORING, float duration = 0.0f, IBotReply* replyWhenAimed = nullptr);
	void AimHeadTowards(edict_t* lookAtTarget, LookPriority priority = BORING, float duration = 0.0f, IBotReply* replyWhenAimed = nullptr);

private:
	CHandle m_lookAtSubject; // Subject to track and look at
	Vector m_lookAtPos; // Position to look at, overriden by the subject position if tracking one
	Vector m_priorAngles; // Last update view angles

	CountdownTimer m_lookAtDuration; // How long to keep looking at
	IntervalTimer m_lookAtTimer; // How long the bot has been looking at
	LookPriority m_lookPriority; // Current active look priority
};

#endif // !BODY_INTERFACE_H_

