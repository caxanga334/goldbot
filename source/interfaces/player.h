#ifndef PLAYER_INTERFACE_H_
#define PLAYER_INTERFACE_H_

#include <extdll.h>
#include <meta_api.h>
#include "sdk/chandle.h"

// IPlayer interface represents the base player class (CBasePlayer)
// It holds data that is valid for any entity that derivates from the CBasePlayer in the mod DLL
// This includes human players and fake clients (bots)

class IPluginBot;

class IPlayer
{
public:
	IPlayer(edict_t* edict);
	virtual ~IPlayer();

	virtual IPluginBot* MyPluginBotPointer() const { return nullptr; }

	virtual bool operator==(const IPlayer& other) const;

	const char* GetName() const;
	edict_t* GetEdict() const;
	entvars_t* GetPev() const;
	const Vector& GetPosition() const;
	const Vector& GetEyePosition() const;
	const Vector& GetEyeAngles() const;
	const Vector& GetVelocity() const;

	void SetEyeAngles(const Vector& angles);

private:
	CHandle m_entity;
};

inline IPlayer::IPlayer(edict_t* edict) :
	m_entity(edict)
{
}

inline IPlayer::~IPlayer()
{
}

inline bool IPlayer::operator==(const IPlayer& other) const
{
	if (this->m_entity == other.m_entity)
		return true;

	return false;
}

inline const char* IPlayer::GetName() const
{
	return STRING(m_entity.Get()->v.netname);
}

inline edict_t* IPlayer::GetEdict() const
{
	return m_entity.Get();
}

inline entvars_t* IPlayer::GetPev() const
{
	return VARS(m_entity.Get());
}

inline const Vector& IPlayer::GetPosition() const
{
	return m_entity.Get()->v.origin;
}

inline const Vector& IPlayer::GetEyePosition() const
{
	static Vector eyePos;
	eyePos = Vector((m_entity.Get()->v.origin + m_entity.Get()->v.view_ofs));
	return eyePos;
}

inline const Vector& IPlayer::GetEyeAngles() const
{
	return m_entity.Get()->v.v_angle;
}

inline const Vector& IPlayer::GetVelocity() const
{
	return m_entity.Get()->v.velocity;
}

inline void IPlayer::SetEyeAngles(const Vector& angles)
{
	m_entity.Get()->v.v_angle = angles;
	// Uncommenter if at some point we start using this on players
	// m_entity.Get()->v.fixangle = 1;
}

#endif // !PLAYER_INTERFACE_H_

