#include <extdll.h>
#include <meta_api.h>
#include "pluginbot.h"
#include "body.h"

IBody::IBody(IPluginBot* bot) : IBotComponent(bot),
	m_lookAtSubject(),
	m_lookAtPos(0.0f, 0.0f, 0.0f),
	m_priorAngles(0.0f, 0.0f, 0.0f),
	m_lookAtDuration(),
	m_lookAtTimer(),
	m_lookPriority(BORING)
{
	m_lookAtTimer.Start();
}

IBody::~IBody()
{
}

void IBody::Reset()
{
	IBotComponent::Reset();
}

void IBody::Update()
{
}

void IBody::UpKeep()
{
	IBotComponent::UpKeep();

	IPluginBot* bot = GetBot();
	// edict_t* edict = bot->GetEdict();

	Vector currentAngles = bot->GetEyeAngles();
	m_priorAngles = currentAngles;

	if (m_lookAtDuration.IsElapsed())
	{
		return;
	}

	if (m_lookAtSubject.IsValid())
	{
		edict_t* subject = m_lookAtSubject.Get();
		IPluginBot* me = GetBot();
		Vector desiredLookAtPos = vec3_origin;
		desiredLookAtPos = me->GetIntentionInterface()->SelectTargetPoint(me, subject);
		m_lookAtPos = desiredLookAtPos;
	}

	Vector to = m_lookAtPos - bot->GetEyePosition();
	Vector desiredAngles;
	VectorAngles(to, desiredAngles);
	Vector angles;

	angles.x = ApproachAngle(desiredAngles.x, currentAngles.x, 30.0f);
	angles.y = ApproachAngle(desiredAngles.y, currentAngles.y, 30.0f);
	angles.z = 0.0f;

	angles.x = AngleNormalize(angles.x);
	angles.y = AngleNormalize(angles.y);

	bot->SetEyeAngles(angles);
}

void IBody::AimHeadTowards(const Vector& lookAtTarget, LookPriority priority, float duration, IBotReply* replyWhenAimed)
{
	if (m_lookPriority == priority)
	{
		if (m_lookAtTimer.IsLessThen(0.2f))
		{
			if (replyWhenAimed)
			{
				replyWhenAimed->OnFailure(GetBot(), IBotReply::DENIED);
			}

			return;
		}
	}

	if (m_lookPriority > priority && !m_lookAtDuration.IsElapsed())
	{
		if (replyWhenAimed)
		{
			replyWhenAimed->OnFailure(GetBot(), IBotReply::DENIED);
		}

		return;
	}

	if (duration < 0.1f)
	{
		duration = 0.1f;
	}

	m_lookAtDuration.Start(duration);
	m_lookAtTimer.Start();
	m_lookAtSubject.Invalidate();
	m_lookAtPos = lookAtTarget;
}

void IBody::AimHeadTowards(edict_t* lookAtTarget, LookPriority priority, float duration, IBotReply* replyWhenAimed)
{
	if (m_lookPriority == priority)
	{
		if (m_lookAtTimer.IsLessThen(0.2f))
		{
			if (replyWhenAimed)
			{
				replyWhenAimed->OnFailure(GetBot(), IBotReply::DENIED);
			}

			return;
		}
	}

	if (m_lookPriority > priority && !m_lookAtDuration.IsElapsed())
	{
		if (replyWhenAimed)
		{
			replyWhenAimed->OnFailure(GetBot(), IBotReply::DENIED);
		}

		return;
	}

	if (duration < 0.1f)
	{
		duration = 0.1f;
	}

	m_lookAtDuration.Start(duration);
	m_lookAtTimer.Start();
	m_lookAtSubject.Set(lookAtTarget);
}
