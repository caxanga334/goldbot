#include <extdll.h>
#include <meta_api.h>
#include "pluginbot.h"
#include "basehllocomotion.h"

constexpr float MAX_TIME_TO_IDLE = 0.10f; // Time since the last movement request the locomotion interface will stop updating the bot's movement controls

CBaseHalfLifeLocomotion::CBaseHalfLifeLocomotion(IPluginBot* bot) : ILocomotion(bot)
{
	FindStepSize();
	m_moveGoal = vec3_origin;
	m_forwardspeed = 0.0f;
	m_sidespeed = 0.0f;
	m_upspeed = 0.0f;
	m_desiredspeed = 0.0f;
}

CBaseHalfLifeLocomotion::~CBaseHalfLifeLocomotion()
{
}

void CBaseHalfLifeLocomotion::Reset()
{
	ILocomotion::Reset();
	m_moveGoal = vec3_origin;

	FindStepSize();
}

void CBaseHalfLifeLocomotion::Update()
{
	ILocomotion::Update();
}

void CBaseHalfLifeLocomotion::UpKeep()
{
	// Reset these
	m_forwardspeed = 0.0f;
	m_sidespeed = 0.0f;
	m_upspeed = 0.0f;

	if (WantsToMove(MAX_TIME_TO_IDLE))
	{
		MoveTowardsGoal();
	}
}

void CBaseHalfLifeLocomotion::Approach(const Vector& goal)
{
	m_moveGoal = goal;
	// Reset these
	m_forwardspeed = 0.0f;
	m_sidespeed = 0.0f;
	m_upspeed = 0.0f;
	ILocomotion::Approach(goal);
	MoveTowardsGoal();
}

void CBaseHalfLifeLocomotion::Run()
{
	m_desiredspeed = GetRunningSpeed();
}

void CBaseHalfLifeLocomotion::Walk()
{
	m_desiredspeed = GetWalkingSpeed();
}

void CBaseHalfLifeLocomotion::Stop()
{
	m_desiredspeed = 0.0f;
}

void CBaseHalfLifeLocomotion::FindStepSize()
{
	m_stepsize = CVAR_GET_FLOAT("sv_stepsize");
}

void CBaseHalfLifeLocomotion::MoveTowardsGoal()
{
	Vector& goal = m_moveGoal;
	IPluginBot* me = GetBot();
	IPlayerInput* playerButtons = me->GetInput();
	Vector forward3D;
	Vector eyeAngles = me->GetEyeAngles();
	AngleVectors(eyeAngles, &forward3D);

	Vector2D forward(forward3D.x, forward3D.y);
	forward.NormalizeInPlace();

	Vector2D right(forward.y, -forward.x);

	Vector2D to = (goal - GetFeet()).Make2D();
	float goalDistance = to.NormalizeInPlace();

	float ahead = to.Dot(forward);
	float side = to.Dot(right);

	constexpr float epsilon = 0.25f;

	if (goalDistance > epsilon)
	{
		playerButtons->SetButtonScale(ahead, side);
	}

	if (ahead > epsilon)
	{
		playerButtons->PressForwardButton();
		m_forwardspeed = m_desiredspeed;
	}
	else if (ahead < -epsilon)
	{
		playerButtons->PressBackwardsButton();
		m_forwardspeed = -m_desiredspeed;
	}

	if (side <= -epsilon)
	{
		playerButtons->PressLeftButton();
		m_sidespeed = -m_desiredspeed;
	}
	else if (side >= epsilon)
	{
		playerButtons->PressRightButton();
		m_sidespeed = m_desiredspeed;
	}
}
