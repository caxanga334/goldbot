#ifndef BASE_HALF_LIFE_LOCOMOTION_H_
#define BASE_HALF_LIFE_LOCOMOTION_H_

#include "locomotion.h"

/**
 * @brief Base implementation of the locomotion interface for the default Half-Life 1 movement style
*/
class CBaseHalfLifeLocomotion : public ILocomotion
{
public:
	CBaseHalfLifeLocomotion(IPluginBot* bot);
	virtual ~CBaseHalfLifeLocomotion();

	virtual void Reset() override;
	virtual void Update() override;
	virtual void UpKeep() override;
	virtual float GetUpdateInterval() override;

	virtual void Approach(const Vector& goal) override;

	virtual void Run() override;
	virtual void Walk() override;
	virtual void Stop() override;

	virtual float GetDesiredSpeed() const override;
	virtual float GetRunningSpeed() const override;
	virtual float GetWalkingSpeed() const override;
	virtual float GetForwardSpeed() const override;
	virtual float GetSideSpeed() const override;
	virtual float GetUpSpeed() const override;
	virtual float GetMaxStepHeigh() const override;
	virtual float GetMaxJumpHeigh() const override;
	virtual float GetDeathDropHeigh() const override;

private:
	void FindStepSize();
	void MoveTowardsGoal();
	float m_stepsize;
	float m_desiredspeed;
	float m_forwardspeed;
	float m_sidespeed;
	float m_upspeed;
	Vector m_moveGoal;
};

inline float CBaseHalfLifeLocomotion::GetUpdateInterval()
{
	return 0.05f;
}

inline float CBaseHalfLifeLocomotion::GetDesiredSpeed() const
{
	return m_desiredspeed;
}

inline float CBaseHalfLifeLocomotion::GetRunningSpeed() const
{
	return 380.0f;
}

inline float CBaseHalfLifeLocomotion::GetWalkingSpeed() const
{
	return 150.0f;
}

inline float CBaseHalfLifeLocomotion::GetForwardSpeed() const
{
	return m_forwardspeed;
}

inline float CBaseHalfLifeLocomotion::GetSideSpeed() const
{
	return m_sidespeed;
}

inline float CBaseHalfLifeLocomotion::GetUpSpeed() const
{
	return m_upspeed;
}

inline float CBaseHalfLifeLocomotion::GetMaxStepHeigh() const
{
	return m_stepsize;
}

inline float CBaseHalfLifeLocomotion::GetMaxJumpHeigh() const
{
	return 52.0f;
}

inline float CBaseHalfLifeLocomotion::GetDeathDropHeigh() const
{
	return 300.0f;
}

#endif // !BASE_HALF_LIFE_LOCOMOTION_H_

