#ifndef BEHAVIOR_INTERFACE_H_
#define BEHAVIOR_INTERFACE_H_

#include <algorithm>
#include <vector>
#include <queue>
#include <string>

#include "eventresponder.h"
#include "contextualquery.h"

template < typename Actor > class Action;

enum ActionResultType
{
	CONTINUE = 0, // Keep the current action running
	CHANGE_TO, // Change to a new action
	SUSPEND_FOR, // Stop processing the current action and starts a new action, return to the previous action when done
	DONE, // End the current action
	SUSTAIN
};

/**
 * @brief The result of a behavior action
 * @tparam Actor Class of the bot that the action belongs to
*/
template < typename Actor >
struct IActionResult
{
	IActionResult(ActionResultType type = CONTINUE, Action< Actor >* action = nullptr, const char* reason = nullptr)
	{
		m_type = type;
		m_action = action;
		m_reason = reason;
	}

	bool IsDone() const
	{
		return (m_type == DONE);
	}

	bool IsContinue() const
	{
		return (m_type == CONTINUE);
	}

	bool IsRequestingChange() const
	{
		return (m_type == CHANGE_TO || m_type == SUSPEND_FOR || m_type == DONE);
	}

	const char* GetTypeName() const
	{
		switch (m_type)
		{
		case CHANGE_TO:		return "CHANGE_TO";
		case SUSPEND_FOR:	return "SUSPEND_FOR";
		case DONE:			return "DONE";
		case SUSTAIN:		return "SUSTAIN";

		case CONTINUE:
		default:
			return "CONTINUE";
		}
	}

	ActionResultType m_type;
	Action< Actor >* m_action;
	const char* m_reason;
};

template < typename Actor >
struct ActionResult : public IActionResult< Actor >
{
	ActionResult(ActionResultType type = CONTINUE, Action< Actor >* action = nullptr, const char* reason = nullptr) : IActionResult< Actor >(type, action, reason) { }
};

enum EventResultPriorityType
{
	RESULT_NONE,
	RESULT_TRY,
	RESULT_IMPORTANT,
	RESULT_CRITICAL
};

template < typename Actor >
struct EventDesiredResult : public IActionResult< Actor >
{
	EventDesiredResult(ActionResultType type = CONTINUE, Action< Actor >* action = nullptr, EventResultPriorityType priority = RESULT_TRY, const char* reason = nullptr) : IActionResult< Actor >(type, action, reason)
	{
		m_priority = priority;
	}

	EventResultPriorityType m_priority;
};

/**
 * @brief The behavior interface is a container for bot actions. It is responsible for manaing and executing actions.
 * @tparam Actor Bot class
*/
template < typename Actor >
class IBehavior : public IEventResponder, public IContextualQuery
{
public:
	IBehavior(Action<Actor>* initialAction, const char* name = nullptr);
	virtual ~IBehavior();

	// Returns true if this behavior doesn't have any actions
	bool IsEmpty() const
	{
		return m_action == nullptr;
	}

	void Reset(Action< Actor >* action)
	{
		if (m_me && m_action)
		{
			m_action->InvokeOnEnd(m_me, this, nullptr);
			m_me = nullptr;
		}

		// find "bottom" action (see comment in destructor)
		Action< Actor >* bottomAction;
		for (bottomAction = m_action; bottomAction && bottomAction->m_buriedUnderMe; bottomAction = bottomAction->m_buriedUnderMe)
			;

		if (bottomAction)
		{
			delete bottomAction;
		}

		while (!m_deadactions.empty())
		{
			Action< Actor >* action = m_deadactions.front();
			delete action;
			m_deadactions.pop();
		}

		m_action = action;
	}

	void Update(Actor* me, float interval)
	{
		if (me == NULL || IsEmpty())
		{
			return;
		}

		m_me = me;
		m_action = m_action->ApplyResult(me, this, m_action->InvokeUpdate(me, this, interval));
		while (!m_deadactions.empty())
		{
			Action< Actor >* action = m_deadactions.front();
			delete action;
			m_deadactions.pop();
		}
	}

	void Resume(Actor* me)
	{
		if (me == NULL || IsEmpty())
		{
			return;
		}

		m_action = m_action->ApplyResult(me, this, m_action->OnResume(me, NULL));
	}

	void DestroyAction(Action< Actor >* dead) { m_deadactions.push(dead); }
	const char* GetName(void) const { return m_name.c_str(); }

	virtual IEventResponder* FirstContainedResponder(void) const override { return m_action; }
	virtual IEventResponder* NextContainedResponder(IEventResponder* current) const override { return nullptr; }


	virtual QueryResultType ShouldPickUp(const IPluginBot* me, edict_t* item) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->ShouldPickUp(me, item);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual QueryResultType ShouldAttack(const IPluginBot* me, const CMemoryEntity* them) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->ShouldAttack(me, them);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual QueryResultType ShouldHurry(const IPluginBot* me) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->ShouldHurry(me);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual QueryResultType ShouldRetreat(const IPluginBot* me) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->ShouldRetreat(me);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual QueryResultType ShouldRoam(const IPluginBot* me) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->ShouldRoam(me);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual QueryResultType ShouldUse(const IPluginBot* me, edict_t* entity) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->ShouldUse(me, entity);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual QueryResultType IsHindrance(const IPluginBot* me, edict_t* blocker) const override
	{
		QueryResultType result = ANSWER_UNDEFINED;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == ANSWER_UNDEFINED)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == ANSWER_UNDEFINED)
				{
					result = action->IsHindrance(me, blocker);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual Vector SelectTargetPoint(const IPluginBot* me, edict_t* subject) const override
	{
		Vector result = vec3_origin;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == vec3_origin)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == vec3_origin)
				{
					result = action->SelectTargetPoint(me, subject);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

	virtual const CMemoryEntity* SelectMoreDangerousThreat(const IPluginBot* me, const CMemoryEntity* threat1, const CMemoryEntity* threat2) const override
	{
		const CMemoryEntity* result = nullptr;

		if (m_action)
		{
			// find innermost child action
			Action< Actor >* action;
			for (action = m_action; action->m_child; action = action->m_child)
				;

			// work our way through our containers
			while (action && result == nullptr)
			{
				Action< Actor >* containingAction = action->m_parent;

				// work our way up the stack
				while (action && result == nullptr)
				{
					result = action->SelectMoreDangerousThreat(me, threat1, threat2);
					action = action->GetActionBuriedUnderMe();
				}

				action = containingAction;
			}
		}

		return result;
	}

private:
	friend class Action<Actor>;
	Action<Actor>* m_action;
	std::string m_name;
	Actor* m_me;
	std::queue<Action<Actor>*> m_deadactions; // Container of actions pending removal
};

template<typename Actor>
IBehavior<Actor>::IBehavior(Action<Actor>* initialAction, const char* name) :
	m_name(name)
{
	m_action = initialAction;
	m_me = nullptr;
}

template<typename Actor>
IBehavior<Actor>::~IBehavior()
{
	if (m_me && m_action)
	{
		m_action->InvokeOnEnd(m_me, this, nullptr);
		m_me = nullptr;
	}

	Action< Actor >* bottomAction;
	for (bottomAction = m_action; bottomAction && bottomAction->m_buriedUnderMe; bottomAction = bottomAction->m_buriedUnderMe)
		;

	if (bottomAction)
	{
		delete bottomAction;
	}

	while (!m_deadactions.empty())
	{
		Action< Actor >* action = m_deadactions.front();
		delete action;
		m_deadactions.pop();
	}
}

template < typename Actor >
class Action : public IEventResponder, public IContextualQuery
{
public:
	Action();
	virtual ~Action();

	virtual const char* GetName() const = 0;
	virtual bool IsNamed(const char* name) const;
	Actor* GetActor() const { return m_actor; }
	virtual ActionResult< Actor > OnStart(Actor* me, Action< Actor >* priorAction) { return Continue(); }
	virtual ActionResult< Actor > Update(Actor* me, float interval) { return Continue(); }
	virtual void OnEnd(Actor* me, Action< Actor >* nextAction) { }
	virtual ActionResult< Actor > OnSuspend(Actor* me, Action< Actor >* interruptingAction) { return Continue(); }
	virtual ActionResult< Actor > OnResume(Actor* me, Action< Actor >* interruptingAction) { return Continue(); }
	ActionResult< Actor > Continue() const;
	ActionResult< Actor > ChangeTo(Action< Actor >* action, const char* reason = nullptr) const;
	ActionResult< Actor > SuspendFor(Action< Actor >* action, const char* reason = nullptr) const;
	ActionResult< Actor > Done(const char* reason = nullptr) const;
	virtual Action< Actor >* InitialContainedAction(Actor* me) { return nullptr; }

	/* These are used to respond to events */

	virtual IEventResponder* FirstContainedResponder() const override;
	virtual IEventResponder* NextContainedResponder(IEventResponder* current) const override;

	virtual EventDesiredResult< Actor > OnStuck(Actor* me) { return TryContinue(); }
	virtual EventDesiredResult< Actor > OnUnstuck(Actor* me) { return TryContinue(); }
	virtual EventDesiredResult< Actor > OnSight(Actor* me , CMemoryEntity* them) { return TryContinue(); }
	virtual EventDesiredResult< Actor > OnLostSight(Actor* me, CMemoryEntity* them) { return TryContinue(); }

	EventDesiredResult< Actor > TryContinue(EventResultPriorityType priority = RESULT_TRY) const;
	EventDesiredResult< Actor > TryChangeTo(Action< Actor >* action, EventResultPriorityType priority = RESULT_TRY, const char* reason = nullptr) const;
	EventDesiredResult< Actor > TrySuspendFor(Action< Actor >* action, EventResultPriorityType priority = RESULT_TRY, const char* reason = nullptr) const;
	EventDesiredResult< Actor > TryDone(EventResultPriorityType priority = RESULT_TRY, const char* reason = nullptr) const;
	EventDesiredResult< Actor > TryToSustain(EventResultPriorityType priority = RESULT_TRY, const char* reason = nullptr) const;

	Action<Actor>* GetActiveChildAction() const { return m_child; }
	Action<Actor>* GetParentAction() const { return m_parent; }
	bool IsSuspended() const { return m_isSuspended; }

private:

#define PROCESS_EVENT( METHOD )							\
		{													\
			if (!m_isStarted)								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while(_action)								\
			{												\
				_result = _action->METHOD(m_actor);	\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if (_action)									\
			{												\
															\
				_action->StorePendingEventResult(_result, #METHOD);	\
			}												\
															\
			IEventResponder::METHOD();			\
		}

#define PROCESS_EVENT_WITH_1_ARG( METHOD, ARG1 )							\
		{													\
			if (!m_isStarted)								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while(_action)								\
			{												\
				_result = _action->METHOD(m_actor, ARG1);	\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if (_action)									\
			{												\
															\
				_action->StorePendingEventResult(_result, #METHOD);	\
			}												\
															\
			IEventResponder::METHOD(ARG1);			\
		}

#define PROCESS_EVENT_WITH_2_ARGS( METHOD, ARG1, ARG2 )							\
		{													\
			if (!m_isStarted)								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while(_action)								\
			{												\
				_result = _action->METHOD(m_actor, ARG1, ARG2);	\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if (_action)									\
			{												\
															\
				_action->StorePendingEventResult(_result, #METHOD);	\
			}												\
															\
			IEventResponder::METHOD(ARG1, ARG2);			\
		}

#define PROCESS_EVENT_WITH_3_ARGS( METHOD, ARG1, ARG2, ARG3 )							\
		{													\
			if (!m_isStarted)								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while(_action)								\
			{												\
				_result = _action->METHOD(m_actor, ARG1, ARG2, ARG3);	\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if (_action)									\
			{												\
															\
				_action->StorePendingEventResult(_result, #METHOD);	\
			}												\
															\
			IEventResponder::METHOD(ARG1, ARG2, ARG3);			\
		}

#define PROCESS_EVENT_WITH_4_ARGS( METHOD, ARG1, ARG2, ARG3, ARG4 )							\
		{													\
			if (!m_isStarted)								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while(_action)								\
			{												\
				_result = _action->METHOD(m_actor, ARG1, ARG2, ARG3, ARG4);	\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if (_action)									\
			{												\
															\
				_action->StorePendingEventResult(_result, #METHOD);	\
			}												\
															\
			IEventResponder::METHOD(ARG1, ARG2, ARG3, ARG4);			\
		}

	virtual void OnStuck() override { PROCESS_EVENT(OnStuck); }
	virtual void OnUnstuck() override { PROCESS_EVENT(OnUnstuck); }
	virtual void OnSight(CMemoryEntity* them) override { PROCESS_EVENT_WITH_1_ARG(OnSight, them); }
	virtual void OnLostSight(CMemoryEntity* them) override { PROCESS_EVENT_WITH_1_ARG(OnLostSight, them); }

	friend class IBehavior<Actor>;
	IBehavior< Actor >* m_behavior;
	Action< Actor >* m_parent;
	Action< Actor >* m_child;

	Action< Actor >* m_buriedUnderMe;
	Action< Actor >* m_coveringMe;

	Actor* m_actor;
	mutable EventDesiredResult< Actor > m_eventResult;
	bool m_isStarted;
	bool m_isSuspended;

	Action< Actor >* GetActionBuriedUnderMe(void) const
	{
		return m_buriedUnderMe;
	}

	Action< Actor >* GetActionCoveringMe(void) const
	{
		return m_coveringMe;
	}

	bool IsOutOfScope(void) const
	{
		for (Action< Actor >* under = GetActionBuriedUnderMe(); under; under = under->GetActionBuriedUnderMe())
		{
			if (under->m_eventResult.m_type == CHANGE_TO ||
				under->m_eventResult.m_type == DONE)
			{
				return true;
			}
		}
		return false;
	}

	ActionResult< Actor > ProcessPendingEvents(void) const
	{
		// if an event has requested a change, honor it
		if (m_eventResult.IsRequestingChange())
		{
			ActionResult< Actor > result(m_eventResult.m_type, m_eventResult.m_action, m_eventResult.m_reason);

			// clear event result in case this change is a suspend and we later resume this action
			m_eventResult = TryContinue(RESULT_NONE);

			return result;
		}

		// check for pending event changes buried in the stack
		Action< Actor >* under = GetActionBuriedUnderMe();
		while (under)
		{
			if (under->m_eventResult.m_type == SUSPEND_FOR)
			{
				// process this pending event in-place and push new Action on the top of the stack
				ActionResult< Actor > result(under->m_eventResult.m_type, under->m_eventResult.m_action, under->m_eventResult.m_reason);

				// clear event result in case this change is a suspend and we later resume this action
				under->m_eventResult = TryContinue(RESULT_NONE);

				return result;
			}

			under = under->GetActionBuriedUnderMe();
		}

		return Continue();
	}

	Action< Actor >* ApplyResult(Actor* me, IBehavior< Actor >* behavior, ActionResult< Actor > result);
	ActionResult< Actor > InvokeOnStart(Actor* me, IBehavior< Actor >* behavior, Action< Actor >* priorAction, Action< Actor >* buriedUnderMeAction);
	ActionResult< Actor > InvokeUpdate(Actor* me, IBehavior< Actor >* behavior, float interval);
	void InvokeOnEnd(Actor* me, IBehavior< Actor >* behavior, Action< Actor >* nextAction);
	Action< Actor >* InvokeOnSuspend(Actor* me, IBehavior< Actor >* behavior, Action< Actor >* interruptingAction);
	ActionResult< Actor > InvokeOnResume(Actor* me, IBehavior< Actor >* behavior, Action< Actor >* interruptingAction);
	void StorePendingEventResult(const EventDesiredResult< Actor >& result, const char* eventName);
	char* BuildDecoratedName(char* name, const Action< Actor >* action) const;
};

template<typename Actor>
Action<Actor>::Action()
{
	m_behavior = nullptr;
	m_parent = nullptr;
	m_child = nullptr;
	m_buriedUnderMe = nullptr;
	m_coveringMe = nullptr;
	m_actor = nullptr;
	m_isStarted = false;
	m_isSuspended = false;
}

template<typename Actor>
Action<Actor>::~Action()
{
	if (m_parent)
	{
		if (m_parent->m_child == this)
		{
			m_parent->m_child = m_buriedUnderMe;
		}
	}

	Action< Actor >* child, * next = nullptr;
	for (child = m_child; child; child = next)
	{
		next = child->m_buriedUnderMe;
		delete child;
	}

	if (m_buriedUnderMe)
	{
		m_buriedUnderMe->m_coveringMe = nullptr;
	}

	if (m_coveringMe)
	{
		delete m_coveringMe;
	}

	if (m_eventResult.m_action)
	{
		delete m_eventResult.m_action;
	}
}

template<typename Actor>
inline Action<Actor>* Action<Actor>::ApplyResult(Actor* me, IBehavior<Actor>* behavior, ActionResult<Actor> result)
{
	Action<Actor>* newAction = result.m_action;
	bool isDebugging = CVAR_GET_FLOAT("gb_debug_enabled") >= 1.0f;

	switch (result.m_type)
	{
	case CHANGE_TO: // Change actions
	{
		if (newAction == NULL)
		{
			LOG_ERROR(PLID, "Error: Attempted CHANGE_TO to a nullptr Action!");
			return this;
		}

		if (isDebugging)
		{
			LOG_MESSAGE(PLID, "%3.2f: %s CHANGE_TO %s", gpGlobals->time, this->GetName(), newAction->GetName());
		}

		this->InvokeOnEnd(me, behavior, newAction);
		ActionResult<Actor> startResult = newAction->InvokeOnStart(me, behavior, this, this->m_buriedUnderMe);

		if (this != newAction)
		{
			behavior->DestroyAction(this);
		}

		return newAction->ApplyResult(me, behavior, startResult);
	}

	case SUSPEND_FOR: // Suspend current action, start a new action
	{
		Action<Actor>* topAction = this;
		while (topAction->m_coveringMe)
		{
			topAction = topAction->m_coveringMe;
		}

		if (isDebugging)
		{
			LOG_MESSAGE(PLID, "%3.2f: %s caused %s SUSPEND_FOR %s", gpGlobals->time, this->GetName(), topAction->GetName(), newAction->GetName());
		}

		topAction = topAction->InvokeOnSuspend(me, behavior, newAction);

		ActionResult< Actor > startResult = newAction->InvokeOnStart(me, behavior, topAction, topAction);

		return newAction->ApplyResult(me, behavior, startResult);
	}

	//-----------------------------------------------------------------------------------------------------
	case DONE:
	{
		Action< Actor >* resumedAction = this->m_buriedUnderMe;

		this->InvokeOnEnd(me, behavior, resumedAction);

		if (isDebugging)
		{
			LOG_MESSAGE(PLID, "%3.2f: %s DONE", gpGlobals->time, this->GetName());

			if (resumedAction)
			{
				LOG_MESSAGE(PLID, "%3.2f: %s", gpGlobals->time, resumedAction->GetName());
			}
		}

		if (resumedAction == nullptr)
		{
			behavior->DestroyAction(this);
			return nullptr;
		}

		ActionResult< Actor > resumeResult = resumedAction->InvokeOnResume(me, behavior, this);

		behavior->DestroyAction(this);
		return resumedAction->ApplyResult(me, behavior, resumeResult);
	}

	case CONTINUE:
	case SUSTAIN:
	default:
		return this;
	}
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::InvokeOnStart(Actor* me, IBehavior<Actor>* behavior, Action<Actor>* priorAction, Action<Actor>* buriedUnderMeAction)
{
	bool isDebugging = CVAR_GET_FLOAT("gb_debug_enabled") >= 1.0f;

	if (isDebugging)
	{
		LOG_MESSAGE(PLID, "%3.2f: STARTING %s", gpGlobals->time, this->GetName());
	}

	// these value must be valid before invoking OnStart, in case an OnSuspend happens 
	m_isStarted = true;
	m_actor = me;
	m_behavior = behavior;

	if (priorAction)
	{
		m_parent = priorAction->m_parent;
	}

	if (m_parent)
	{
		m_parent->m_child = this;
	}

	m_buriedUnderMe = buriedUnderMeAction;
	if (buriedUnderMeAction)
	{
		buriedUnderMeAction->m_coveringMe = this;
	}

	m_coveringMe = nullptr;

	m_child = InitialContainedAction(me);
	if (m_child)
	{
		m_child->m_parent = this;
		m_child = m_child->ApplyResult(me, behavior, ChangeTo(m_child, "Starting child Action"));
	}

	ActionResult<Actor> result = OnStart(me, priorAction);

	return result;
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::InvokeUpdate(Actor* me, IBehavior<Actor>* behavior, float interval)
{
	if (IsOutOfScope())
	{
		return Done("Out of scope");
	}

	if (!m_isStarted)
	{
		return ChangeTo(this, "Starting Action");
	}

	ActionResult< Actor > eventResult = ProcessPendingEvents();
	if (!eventResult.IsContinue())
	{
		return eventResult;
	}

	if (m_child)
	{
		m_child = m_child->ApplyResult(me, behavior, m_child->InvokeUpdate(me, behavior, interval));
	}

	ActionResult< Actor > result;
	{
		result = Update(me, interval);
	}

	return result;
}

template<typename Actor>
inline void Action<Actor>::InvokeOnEnd(Actor* me, IBehavior<Actor>* behavior, Action<Actor>* nextAction)
{
	if (!m_isStarted)
	{
		return;
	}

	bool isDebugging = CVAR_GET_FLOAT("gb_debug_enabled") >= 1.0f;

	if (isDebugging)
	{
		LOG_MESSAGE(PLID, "%3.2f: %s ENDING %s", gpGlobals->time, behavior->GetName(), this->GetName());
	}

	m_isStarted = false;

	Action< Actor >* child, * next = nullptr;
	for (child = m_child; child; child = next)
	{
		next = child->m_buriedUnderMe;
		child->InvokeOnEnd(me, behavior, nextAction);
	}

	OnEnd(me, nextAction);

	if (m_coveringMe)
	{
		m_coveringMe->InvokeOnEnd(me, behavior, nextAction);
	}
}

template<typename Actor>
inline Action<Actor>* Action<Actor>::InvokeOnSuspend(Actor* me, IBehavior<Actor>* behavior, Action<Actor>* interruptingAction)
{
	bool isDebugging = CVAR_GET_FLOAT("gb_debug_enabled") >= 1.0f;

	if (isDebugging)
	{
		LOG_MESSAGE(PLID, "%3.2f: %s SUSPENDING %s", gpGlobals->time, behavior->GetName(), this->GetName());
	}

	if (m_child)
	{
		m_child = m_child->InvokeOnSuspend(me, behavior, interruptingAction);
	}

	m_isSuspended = true;
	ActionResult< Actor > result = OnSuspend(me, interruptingAction);

	if (result.IsDone())
	{
		InvokeOnEnd(me, behavior, nullptr);
		Action< Actor >* buried = GetActionBuriedUnderMe();
		behavior->DestroyAction(this);
		return buried;
	}

	return this;
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::InvokeOnResume(Actor* me, IBehavior<Actor>* behavior, Action<Actor>* interruptingAction)
{
	bool isDebugging = CVAR_GET_FLOAT("gb_debug_enabled") >= 1.0f;

	if (isDebugging)
	{
		LOG_MESSAGE(PLID, "%3.2f: %s RESUMING %s", gpGlobals->time, behavior->GetName(), this->GetName());
	}

	if (!m_isSuspended)
	{
		return Continue();
	}

	if (m_eventResult.IsRequestingChange())
	{
		return Continue();
	}

	m_isSuspended = false;
	m_coveringMe = nullptr;

	if (m_parent)
	{
		m_parent->m_child = this;
	}

	if (m_child)
	{
		m_child = m_child->ApplyResult(me, behavior, m_child->InvokeOnResume(me, behavior, interruptingAction));
	}

	ActionResult< Actor > result = OnResume(me, interruptingAction);

	return result;
}

template<typename Actor>
inline void Action<Actor>::StorePendingEventResult(const EventDesiredResult<Actor>& result, const char* eventName)
{
	if (result.IsContinue())
	{
		return;
	}

	if (result.m_priority >= m_eventResult.m_priority)
	{
		if (m_eventResult.m_priority == RESULT_CRITICAL)
		{
			if (CVAR_GET_FLOAT("gb_debug_enabled") >= 1.0f)
			{
				LOG_CONSOLE(PLID, "%3.2f: Warning: %s::%s RESULT_CRITICAL collision!", gpGlobals->time, GetName(), eventName);
			}
		}

		if (m_eventResult.m_action)
		{
			delete m_eventResult.m_action;
		}

		m_eventResult = result;
	}
	else
	{
		if (result.m_action)
		{
			delete result.m_action;
		}
	}
}

template<typename Actor>
inline char* Action<Actor>::BuildDecoratedName(char* name, const Action<Actor>* action) const
{
	strcat(name, action->GetName());

	const Action< Actor >* child = action->GetActiveChildAction();
	if (child)
	{
		strcat(name, "( ");
		BuildDecoratedName(name, child);
		strcat(name, " )");
	}

	const Action< Actor >* buried = action->GetActionBuriedUnderMe();
	if (buried)
	{
		strcat(name, "<<");
		BuildDecoratedName(name, buried);
	}

	return name;
}

template<typename Actor>
inline bool Action<Actor>::IsNamed(const char* name) const
{
	return strcmp(GetName(), name) == 0;
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::Continue(void) const
{
	return ActionResult<Actor>(CONTINUE, nullptr, nullptr);
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::ChangeTo(Action<Actor>* action, const char* reason) const
{
	return ActionResult<Actor>(CHANGE_TO, action, reason);
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::SuspendFor(Action<Actor>* action, const char* reason) const
{
	m_eventResult = TryContinue(RESULT_NONE);
	return ActionResult<Actor>(SUSPEND_FOR, action, reason);
}

template<typename Actor>
inline ActionResult<Actor> Action<Actor>::Done(const char* reason) const
{
	return ActionResult<Actor>(DONE, nullptr, reason);
}

template<typename Actor>
inline IEventResponder* Action<Actor>::FirstContainedResponder(void) const
{
	return GetActiveChildAction();
}

template<typename Actor>
inline IEventResponder* Action<Actor>::NextContainedResponder(IEventResponder* current) const
{
	return nullptr;
}

template<typename Actor>
inline EventDesiredResult<Actor> Action<Actor>::TryContinue(EventResultPriorityType priority) const
{
	return EventDesiredResult<Actor>(CONTINUE, nullptr, priority);
}

template<typename Actor>
inline EventDesiredResult<Actor> Action<Actor>::TryChangeTo(Action<Actor>* action, EventResultPriorityType priority, const char* reason) const
{
	return EventDesiredResult<Actor>(CHANGE_TO, action, priority, reason);
}

template<typename Actor>
inline EventDesiredResult<Actor> Action<Actor>::TrySuspendFor(Action<Actor>* action, EventResultPriorityType priority, const char* reason) const
{
	return EventDesiredResult<Actor>(SUSPEND_FOR, action, priority, reason);
}

template<typename Actor>
inline EventDesiredResult<Actor> Action<Actor>::TryDone(EventResultPriorityType priority, const char* reason) const
{
	return EventDesiredResult<Actor>(DONE, nullptr, priority, reason);
}

template<typename Actor>
inline EventDesiredResult<Actor> Action<Actor>::TryToSustain(EventResultPriorityType priority, const char* reason) const
{
	return EventDesiredResult<Actor>(SUSTAIN, nullptr, priority, reason);
}


#endif // !BEHAVIOR_INTERFACE_H_
