#ifndef INTENTION_INTERFACE_H_
#define INTENTION_INTERFACE_H_

#include "component.h"
#include "contextualquery.h"

class IIntention : public IBotComponent, public IContextualQuery
{
public:
	IIntention(IPluginBot* bot);
	virtual ~IIntention();

	virtual void Reset() override { IBotComponent::Reset(); }
	virtual void Update() override {}
	virtual void UpKeep() override { IBotComponent::UpKeep(); }

	virtual QueryResultType ShouldPickUp(const IPluginBot* me, edict_t* item) const override;
	virtual QueryResultType ShouldAttack(const IPluginBot* me, const CMemoryEntity* them) const override;
	virtual QueryResultType ShouldHurry(const IPluginBot* me) const override;
	virtual QueryResultType ShouldRetreat(const IPluginBot* me) const override;
	virtual QueryResultType ShouldRoam(const IPluginBot* me) const override;
	virtual QueryResultType ShouldUse(const IPluginBot* me, edict_t* entity) const override;
	virtual QueryResultType IsHindrance(const IPluginBot* me, edict_t* blocker) const override;
	virtual Vector SelectTargetPoint(const IPluginBot* me, edict_t* subject) const override;
	virtual const CMemoryEntity* SelectMoreDangerousThreat(const IPluginBot* me, const CMemoryEntity* threat1, const CMemoryEntity* threat2) const override;
};

inline QueryResultType IIntention::ShouldPickUp(const IPluginBot* me, edict_t* item) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->ShouldPickUp(me, item);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

inline QueryResultType IIntention::ShouldAttack(const IPluginBot* me, const CMemoryEntity* them) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->ShouldAttack(me, them);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

inline QueryResultType IIntention::ShouldHurry(const IPluginBot* me) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->ShouldHurry(me);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

inline QueryResultType IIntention::ShouldRetreat(const IPluginBot* me) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->ShouldRetreat(me);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

inline QueryResultType IIntention::ShouldRoam(const IPluginBot* me) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->ShouldRoam(me);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

inline QueryResultType IIntention::ShouldUse(const IPluginBot* me, edict_t* entity) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->ShouldUse(me, entity);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

inline QueryResultType IIntention::IsHindrance(const IPluginBot* me, edict_t* blocker) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			QueryResultType result = query->IsHindrance(me, blocker);
			if (result != ANSWER_UNDEFINED)
			{
				return result;
			}
		}
	}
	return ANSWER_UNDEFINED;
}

#endif // !INTENTION_INTERFACE_H_

