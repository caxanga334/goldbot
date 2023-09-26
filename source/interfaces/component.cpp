#include <extdll.h>
#include <meta_api.h>
#include "pluginglobals.h"
#include "pluginbot.h"
#include "component.h"

IBotComponent::IBotComponent(IPluginBot* bot)
{
	m_nextUpdateTick = -1;
	m_bot = bot;
	m_nextComponent = nullptr;
	bot->RegisterComponent(this);
}

IBotComponent::~IBotComponent()
{
}

bool IBotComponent::ShouldUpdate()
{
	if (m_nextUpdateTick <= 0)
	{
		m_nextUpdateTick = TIME_TO_TICKS(GetUpdateInterval());
		return true;
	}

	return false;
}
