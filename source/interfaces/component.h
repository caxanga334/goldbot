#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "eventresponder.h"

class IPluginBot;

/**
 * @brief Interface for replies between bot components and interfaces
*/
class IBotReply
{
public:
	virtual void OnSuccess(IPluginBot* bot) {}

	enum FailureReason
	{
		DENIED = 0, // The task was denied
		INTERRUPTED, // The task was interrupted by something else
		FAILED // The task failed
	};

	virtual void OnFailure(IPluginBot* bot, FailureReason reason) {}
};

class IBotComponent : public IEventResponder
{
public:
	IBotComponent(IPluginBot* bot);
	virtual ~IBotComponent();

	virtual void Reset() { m_nextUpdateTick = -1; }
	virtual void Update() = 0;
	virtual void UpKeep();

	virtual IPluginBot* GetBot() const { return m_bot; }
	virtual float GetUpdateInterval();

	bool ShouldUpdate();

private:
	int m_nextUpdateTick;

	friend class IPluginBot;

	IPluginBot* m_bot;
	IBotComponent* m_nextComponent;
};

inline void IBotComponent::UpKeep()
{
	if (m_nextUpdateTick > 0)
	{
		m_nextUpdateTick--;
	}
}

inline float IBotComponent::GetUpdateInterval()
{
	return 0.1f; // Components update every 100 ms by default
}

#endif // !COMPONENT_H_

