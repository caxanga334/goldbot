#include <extdll.h>
#include <meta_api.h>
#include "pluginbot.h"
#include "mods/mod_base.h"
#include "locomotion.h"

constexpr auto STUCK_RADIUS = 100.0f * 100.0f; // squared distance

void ILocomotion::Reset()
{
	IBotComponent::Reset();
	ClearStuckStatus();
	m_motionVector = Vector(1.0f, 0.0f, 0.0f);
	m_movementRequestTimer.Invalidate();
}

void ILocomotion::Update()
{
	StuckMonitor();
}

void ILocomotion::ClearStuckStatus()
{
	if (IsStuck())
	{
		GetBot()->OnUnstuck();
	}

	m_isStuck = false;
	m_stuckPos = GetBot()->GetPosition();
	m_stuckTimer.Reset();
	m_stillStuckTimer.Invalidate();
}

void ILocomotion::StuckMonitor()
{
	constexpr auto idletime = 0.25f;
	IPluginBot* me = GetBot();

	if (m_movementRequestTimer.IsGreaterThen(idletime))
	{
		m_stuckPos = me->GetPosition();
		m_stuckTimer.Reset();
		return;
	}

	if (IsStuck())
	{
		if (GetBot()->GetRangeToSqr(m_stuckPos) > STUCK_RADIUS)
		{
			ClearStuckStatus();
		}
		else
		{
			if (m_stillStuckTimer.IsElapsed())
			{
				m_stillStuckTimer.Start(1.0f);
				GetBot()->OnStuck();
			}
		}
	}
	else
	{
		if (GetBot()->GetRangeToSqr(m_stuckPos) > STUCK_RADIUS)
		{
			m_stuckPos = GetBot()->GetPosition();
			m_stuckTimer.Start();
		}
		else
		{
			float minMoveSpeed = 0.1f * GetDesiredSpeed() + 0.1f;
			float escapeTime = STUCK_RADIUS / minMoveSpeed;

			if (m_stuckTimer.IsGreaterThen(escapeTime))
			{
				m_isStuck = true;
				GetBot()->OnStuck();
			}
		}
	}
}

const Vector& ILocomotion::GetFeet() const
{
	static Vector feet;
	feet = GetBot()->GetPev()->origin;
	feet.z = feet.z - gamemod->GetPlayerOriginGroundOffset();
	return feet;
}
