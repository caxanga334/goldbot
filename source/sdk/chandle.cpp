#include <extdll.h>
#include <meta_api.h>
#include "chandle.h"

CHandle::CHandle()
{
	m_pent = nullptr;
	m_serialnumber = -1;
	m_index = -1;
}

CHandle::CHandle(edict_t* edict)
{
	if (edict == nullptr)
	{
		m_pent = nullptr;
		m_serialnumber = -1;
		m_index = -1;
	}
	else
	{
		m_pent = edict;
		m_serialnumber = edict->serialnumber;
		m_index = ENTINDEX(edict);
	}
}

CHandle::operator void* ()
{
	if (m_pent)
	{
		if (m_pent->serialnumber == m_serialnumber && m_index == ENTINDEX(m_pent))
		{
			return m_pent;
		}
	}

	return nullptr;
}

void CHandle::operator=(edict_t* edict)
{
	m_pent = edict;
	m_serialnumber = edict->serialnumber;
	m_index = ENTINDEX(edict);
}

void CHandle::operator=(CHandle& other)
{
	this->m_pent = other.m_pent;
	this->m_serialnumber = other.m_serialnumber;
	this->m_index = other.m_index;
}

bool CHandle::operator==(edict_t* edict)
{
	// Gold Source jankiness, most edicts seem to have a serial number of 0, for an edict be the same, the serial number and index must be the same
	// IDK how the SDK's EHANDLE works, probably being used in a different manner
	// Should've coded a bot for the Source Engine.

	if (this->m_pent == nullptr)
	{
		return false;
	}
	else if (this->m_serialnumber == edict->serialnumber && m_index == ENTINDEX(edict))
	{
		return true;
	}

	return false;
}

edict_t* CHandle::Get() const
{
	if (m_pent)
	{
		// ENTINDEX might be useless here
		if (m_pent->serialnumber == m_serialnumber && m_index == ENTINDEX(m_pent))
		{
			return m_pent;
		}
	}

	return nullptr;
}

void CHandle::Set(edict_t* edict)
{
	m_pent = edict;
	m_serialnumber = edict->serialnumber;
	m_index = ENTINDEX(edict);
}

bool CHandle::IsValid() const
{
	return m_pent != nullptr && m_pent->serialnumber == m_serialnumber && m_index == ENTINDEX(m_pent);
}

