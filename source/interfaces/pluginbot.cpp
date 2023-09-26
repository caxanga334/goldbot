#include <extdll.h>
#include <meta_api.h>

#include "sdk/math_vectors.h"
#include "waypoints/waypoint_manager.h"
#include "manager.h"
#include "plugincvars.h"
#include "pluginglobals.h"
#include "pluginutil.h"
#include "pluginbot.h"

IPluginBot::IPluginBot(edict_t* pEdict) : IPlayer(pEdict),
	m_input()
{
	m_lastusercmdtime = 0.0f;
	m_nextUpdateTick = -1;
	m_componentList = nullptr;
	m_basevision = nullptr;
	m_basebody = nullptr;
	m_baseintention = nullptr;
	m_baselocomotion = nullptr;

	TheBotManager->RegisterBot(this);
}

IPluginBot::~IPluginBot()
{
	if (m_basevision)
	{
		delete m_basevision;
	}

	if (m_basebody)
	{
		delete m_basebody;
	}

	if (m_baseintention)
	{
		delete m_baseintention;
	}

	if (m_baselocomotion)
	{
		delete m_baselocomotion;
	}

	TheBotManager->UnregisterBot(this);
}

bool IPluginBot::BeginUpdate()
{
	if (m_nextUpdateTick <= 0)
	{
		Update();
		m_nextUpdateTick = TIME_TO_TICKS(CVAR_GET_FLOAT("gb_update_rate"));
		return true;
	}

	m_nextUpdateTick--;
	return false;
}

IPluginBot* IPluginBot::MyPluginBotPointer() const
{
	return const_cast<IPluginBot*>(this);
}

void IPluginBot::Reset()
{
	for (IBotComponent* component = m_componentList; component; component = component->m_nextComponent)
	{
		component->Reset();
	}

	m_nextUpdateTick = -1;
	m_lastusercmdtime = gpGlobals->time;
}

void IPluginBot::Update()
{
	for (IBotComponent* component = m_componentList; component; component = component->m_nextComponent)
	{
		if (component->ShouldUpdate())
		{
			component->Update();
		}
	}
}

void IPluginBot::UpKeep()
{
	for (IBotComponent* component = m_componentList; component; component = component->m_nextComponent)
	{
		component->UpKeep();
	}

	RunPlayerMove();
}

void IPluginBot::RunPlayerMove()
{
	// https://github.com/ValveSoftware/halflife/blob/master/game_shared/bot/bot.cpp#L338

	edict_t* edict = GetEdict();
	ILocomotion* mover = GetLocomotionInterface();
	float forwardspeed = mover->GetForwardSpeed();
	float sidespeed = mover->GetSideSpeed();
	float upspeed = mover->GetUpSpeed();
	int inputButtons = m_input.GetInputButtons();
	auto msec = GetUsercmdMsec();
	m_lastusercmdtime = gpGlobals->time;

	edict->v.angles = edict->v.v_angle;
	edict->v.angles.x /= -3.0f;

	g_engfuncs.pfnRunPlayerMove(edict, edict->v.v_angle, forwardspeed, sidespeed, upspeed, inputButtons, m_input.GetImpulse(), msec);
}

IEventResponder* IPluginBot::FirstContainedResponder(void) const
{
	return m_componentList;
}

IEventResponder* IPluginBot::NextContainedResponder(IEventResponder* current) const
{
	return static_cast<IBotComponent*>(current)->m_nextComponent;
}

void IPluginBot::OnSight(CMemoryEntity* them)
{
	auto cls = STRING(them->GetEdict()->v.classname);
	LOG_MESSAGE(PLID, "%3.2f Bot caugh line of sight of entity %s#%i(%i)", gpGlobals->time, cls, ENTINDEX(them->GetEdict()), them->GetEdict()->serialnumber);

	auto& start = GetPosition();
	auto& end = them->GetLastKnownPosition();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, nullptr, static_cast<edict_t*>(nullptr));
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(start[0]);
	WRITE_COORD(start[1]);
	WRITE_COORD(start[2]);
	WRITE_COORD(end[0]);
	WRITE_COORD(end[1]);
	WRITE_COORD(end[2]);
	WRITE_SHORT(TheWaypoints->GetSpriteTexture());
	WRITE_BYTE(1); // framestart
	WRITE_BYTE(10); // framerate
	WRITE_BYTE(200); // life in 0.1's
	WRITE_BYTE(30); // width
	WRITE_BYTE(0);  // noise
	WRITE_BYTE(0);   // r, g, b
	WRITE_BYTE(255);   // r, g, b
	WRITE_BYTE(0);   // r, g, b
	WRITE_BYTE(255);   // brightness
	WRITE_BYTE(5);    // speed
	MESSAGE_END();

	IEventResponder::OnSight(them);
}

void IPluginBot::OnLostSight(CMemoryEntity* them)
{
	auto cls = STRING(them->GetEdict()->v.classname);
	LOG_MESSAGE(PLID, "%3.2f Bot lost light of sight of entity %s#%i", gpGlobals->time, cls, ENTINDEX(them->GetEdict()));

	UTIL_DrawBeamPoints(GetEyePosition(), them->GetEdict()->v.origin, 200, 255, 0, 0);
	IEventResponder::OnLostSight(them);
}

IBody* IPluginBot::GetBodyInterface() const
{
	if (!m_basebody)
	{
		m_basebody = new IBody(const_cast<IPluginBot*>(this));
	}

	return m_basebody;
}

IVision* IPluginBot::GetVisionInterface() const
{
	if (!m_basevision)
	{
		m_basevision = new IVision(const_cast<IPluginBot*>(this));
	}

	return m_basevision;
}

IIntention* IPluginBot::GetIntentionInterface() const
{
	if (!m_baseintention)
	{
		m_baseintention = new IIntention(const_cast<IPluginBot*>(this));
	}

	return m_baseintention;
}

ILocomotion* IPluginBot::GetLocomotionInterface() const
{
	if (!m_baselocomotion)
	{
		m_baselocomotion = new ILocomotion(const_cast<IPluginBot*>(this));
	}

	return m_baselocomotion;
}

byte IPluginBot::GetUsercmdMsec()
{
	// https://github.com/SamVanheer/halflife-updated/blob/master/game_shared/bot/bot.cpp#L372

	float msec = (gpGlobals->time - m_lastusercmdtime) * 1000.0f;
	msec = std::clamp(msec, 1.0f, 255.0f);
	return static_cast<byte>(msec);
}

void IPluginBot::RegisterComponent(IBotComponent* component)
{
	component->m_nextComponent = m_componentList;
	m_componentList = component;
}
