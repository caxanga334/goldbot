#ifndef EVENT_RESPONDER_H_
#define EVENT_RESPONDER_H_

enum MoveToFailureType
{
	FAIL_NO_PATH_EXISTS, // A* couldn't find a path
	FAIL_STUCK, // The bot is stuck
	FAIL_FELL_OFF, // The bot fell off the path
};

class CMemoryEntity;

class IEventResponder
{
public:
	virtual ~IEventResponder();

	// These methods are overriden by derivated classes to determine how events propagate
	virtual IEventResponder* FirstContainedResponder(void) const { return nullptr; }
	virtual IEventResponder* NextContainedResponder(IEventResponder* current) const { return nullptr; }

	virtual void OnStuck();
	virtual void OnUnstuck();
	virtual void OnSight(CMemoryEntity* them);
	virtual void OnLostSight(CMemoryEntity* them);
};

inline void IEventResponder::OnStuck()
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnStuck();
	}
}

inline void IEventResponder::OnUnstuck()
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnUnstuck();
	}
}

inline void IEventResponder::OnSight(CMemoryEntity* them)
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnSight(them);
	}
}

inline void IEventResponder::OnLostSight(CMemoryEntity* them)
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnLostSight(them);
	}
}

#endif // !EVENT_RESPONDER_H_

