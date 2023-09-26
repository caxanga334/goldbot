#include <extdll.h>
#include <meta_api.h>
#include "pluginbot.h"
#include "waypoints/waypoint_base.h"
#include "waypoints/waypoint_manager.h"
#include "memoryentity.h"

CMemoryEntity::CMemoryEntity(edict_t* edict) :
	m_lastknownposition(),
	m_lastknownvelocity()
{
	m_handle = edict;
	m_timesinceknown = gpGlobals->time;
	m_lastknownwaypoint = nullptr;
	m_lpkwasseen = false;
	m_visible = false;
	UpdatePosition();
}

void CMemoryEntity::UpdatePosition()
{
	if (m_handle.IsValid())
	{
		m_timesincelastseen = gpGlobals->time;
		m_lastknownposition = Vector(m_handle->v.origin);
		m_lastknownvelocity = Vector(m_handle->v.velocity);
		m_lastknownwaypoint = TheWaypoints->GetNearestWaypoint(m_lastknownposition);
	}
}

void CMemoryEntity::UpdateVisibleStatus(const bool visible)
{
	if (visible)
	{
		m_visible = true;
		m_lpkwasseen = true;
		UpdatePosition();
	}
	else
	{
		m_visible = false;
	}
}

bool CMemoryEntity::IsObsolete() const
{
	if (!m_handle.IsValid())
		return true;

	float difflastseen = gpGlobals->time - m_timesincelastseen;

	// forget after 3 seconds if i've seen thier last known position
	if (difflastseen > 3.0f && m_lpkwasseen == true)
		return true;

	return difflastseen > 15.0f; // forget about memorized entities after 15 seconds
}

bool CMemoryEntity::WasVisibleRecently() const
{
	if (m_visible)
		return true;

	if (GetTimeSinceLastSeen() < 3.0f)
		return true;

	return false;
}

float CMemoryEntity::GetTimeSinceLastSeen() const
{
	return gpGlobals->time - m_timesincelastseen;
}

float CMemoryEntity::GetTimeSinceMemorized() const
{
	return gpGlobals->time - m_timesinceknown;
}

bool CMemoryEntity::operator==(const CMemoryEntity& other) const
{
	if (this->m_handle == other.m_handle)
		return true;

	return false;
}

void CMemoryEntity::operator=(const CMemoryEntity& other)
{
	this->m_handle.Set(other.GetEdict());
	this->UpdatePosition();
}

