#ifndef LOCOMOTION_INTERFACE_H_
#define LOCOMOTION_INTERFACE_H_

#include "sdk/timers.h"
#include "component.h"

class CWaypoint;
class IPluginBot;

class ILocomotion : public IBotComponent
{
public:
	ILocomotion(IPluginBot* bot);
	virtual ~ILocomotion();

	virtual void Reset() override;
	virtual void Update() override;

	virtual void Approach(const Vector& goal) { m_movementRequestTimer.Start(); }
	virtual void Run() {} // Set movement speed to running
	virtual void Walk() {} // Set movement speed to walking
	virtual void Stop() {} // Stop moving
	virtual void Jump() {}

	virtual float GetDesiredSpeed() const;
	virtual float GetRunningSpeed() const;
	virtual float GetWalkingSpeed() const;
	virtual float GetForwardSpeed() const;
	virtual float GetSideSpeed() const;
	virtual float GetUpSpeed() const;
	virtual float GetMaxStepHeigh() const;
	virtual float GetMaxJumpHeigh() const;
	virtual float GetDeathDropHeigh() const;

	virtual void SetDesiredSpeed(float speed) {}
	virtual const Vector& GetFeet() const;

	virtual bool IsWaypointTraversable(const CWaypoint* waypoint) const { return true; }

	const bool IsStuck() const { return m_isStuck; }
	const float GetStuckDuration() const { return m_stuckTimer.GetElapsedTime(); }
	void ClearStuckStatus();
	const bool WantsToMove(const float idletime) { return m_movementRequestTimer.IsLessThen(idletime); }

private:
	void StuckMonitor();

	bool m_isStuck;
	Vector m_motionVector;
	Vector m_stuckPos;
	IntervalTimer m_stuckTimer;
	IntervalTimer m_movementRequestTimer;
	CountdownTimer m_stillStuckTimer;
};

inline ILocomotion::ILocomotion(IPluginBot* bot) : IBotComponent(bot),
	m_stuckPos(0.0f, 0.0f, 0.0f)
{
	m_motionVector = Vector(1.0f, 0.0f, 0.0f);
	m_isStuck = false;
}

inline ILocomotion::~ILocomotion()
{
}

inline float ILocomotion::GetDesiredSpeed() const
{
	return 0.0f;
}

inline float ILocomotion::GetRunningSpeed() const
{
	return 0.0f;
}

inline float ILocomotion::GetWalkingSpeed() const
{
	return 0.0f;
}

inline float ILocomotion::GetForwardSpeed() const
{
	return 0.0f;
}

inline float ILocomotion::GetSideSpeed() const
{
	return 0.0f;
}

inline float ILocomotion::GetUpSpeed() const
{
	return 0.0f;
}

inline float ILocomotion::GetMaxStepHeigh() const
{
	return 0.0f;
}

inline float ILocomotion::GetMaxJumpHeigh() const
{
	return 0.0f;
}

inline float ILocomotion::GetDeathDropHeigh() const
{
	return 0.0f;
}

#endif // !LOCOMOTION_INTERFACE_H_

