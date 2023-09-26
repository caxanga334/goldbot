#include <extdll.h>
#include <meta_api.h>
#include "pluginutil.h"
#include "pluginbot.h"
#include "vision.h"

IVision::IVision(IPluginBot* bot) : IBotComponent(bot),
	m_knownentities()
{
	SetFieldOfView(GetDefaultFieldOfView());
}

IVision::~IVision()
{
}

void IVision::Reset()
{
	SetFieldOfView(GetDefaultFieldOfView());
	IBotComponent::Reset();
}

void IVision::Update()
{
	// Erase obsolete entities, this will also erase any invalid entity
	if (m_knownentities.size() > 0)
	{
		m_knownentities.erase(std::remove_if(m_knownentities.begin(), m_knownentities.end(), [](CMemoryEntity& value) { return value.IsObsolete(); }), m_knownentities.end());
	}

	// Update the visible status of entities already in the list
	UpdateVisibleEntities();

	// Update the list with new entities
	CollectVisibleEntities();
}

void IVision::UpKeep()
{
	IBotComponent::UpKeep();
}

void IVision::SetFieldOfView(float angle)
{
	m_fov = angle;
	m_coshalf = cos(0.5f * angle * M_PI / 180.0f);
}

int IVision::GetMaxSearchEnts() const
{
	return gpGlobals->maxEntities;
}

bool IVision::IsInFieldOfView(const Vector& target) const
{
	auto source = GetBot()->GetEyePosition();
	Vector forward;
	AngleVectors(GetBot()->GetEyeAngles(), &forward);

	return PointWithinViewAngle(source, target, forward, m_coshalf);
}

bool IVision::IsInFieldOfView(const edict_t* edict) const
{
	auto source = GetBot()->GetEyePosition();
	auto target = Vector(edict->v.origin);
	Vector forward;
	AngleVectors(GetBot()->GetEyeAngles(), &forward);

	return PointWithinViewAngle(source, target, forward, m_coshalf);
}

bool IVision::IsAwareOf(const CMemoryEntity& other)
{
	auto position = std::find(m_knownentities.begin(), m_knownentities.end(), other);
	return position != m_knownentities.end();
}

void IVision::UpdateVisibleEntities()
{
	for (auto& known : m_knownentities)
	{
		auto pos = Vector(known.GetEdict()->v.origin);

		if (IsInFieldOfView(pos))
		{
			known.UpdateVisibleStatus(true);
		}
		else
		{
			if (known.IsVisibleRightNow())
			{
				GetBot()->OnLostSight(&known);
			}

			known.UpdateVisibleStatus(false);
		}
	}
}

// Collects and 
void IVision::CollectVisibleEntities()
{
	int maxEnts = GetMaxSearchEnts();
	for (int i = 1; i <= maxEnts; i++)
	{
		edict_t* edict = g_engfuncs.pfnPEntityOfEntIndex(i);
		auto me = GetBot()->GetEdict();

		if (edict == nullptr)
			continue;

		if (edict == me)
			continue;

		if (edict->free)
			continue;

		// This is important, player edicts are reserved and always valid, except there is no private data allocated on empty player slots
		if (edict->pvPrivateData == nullptr)
			continue;

		if (FNullEnt(edict))
			continue;

		if (IsIgnored(edict))
			continue;

		CMemoryEntity them(edict);

		if (IsAwareOf(them))
			continue;

		if (IsInFieldOfView(edict))
		{
			m_knownentities.push_back(them);
			GetBot()->OnSight(&them);
		}
	}
}
