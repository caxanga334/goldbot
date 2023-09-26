#include <extdll.h>
#include <meta_api.h>
#include "pluginbot.h"
#include "pluginutil.h"
#include "intention.h"

IIntention::IIntention(IPluginBot* bot) : IBotComponent(bot)
{
}

IIntention::~IIntention()
{
}

Vector IIntention::SelectTargetPoint(const IPluginBot* me, edict_t* subject) const
{
	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			Vector result = query->SelectTargetPoint(me, subject);
			if (result != vec3_origin)
			{
				return result;
			}
		}
	}

	return UTIL_ComputeOrigin(subject);
}

const CMemoryEntity* IIntention::SelectMoreDangerousThreat(const IPluginBot* me, const CMemoryEntity* threat1, const CMemoryEntity* threat2) const
{
	if (!threat1 || threat1->IsObsolete())
	{
		if (threat2 && !threat2->IsObsolete())
			return threat2;

		return nullptr;
	}
	else if (!threat2 || threat2->IsObsolete())
	{
		return threat1;
	}

	for (IEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		const IContextualQuery* query = dynamic_cast<const IContextualQuery*>(sub);
		if (query)
		{
			// return the response of the first responder that gives a definitive answer
			const CMemoryEntity* result = query->SelectMoreDangerousThreat(me, threat1, threat2);
			if (result)
			{
				return result;
			}
		}
	}

	// No decision was made
	float range1 = me->GetRangeToSqr(threat1->GetLastKnownPosition());
	float range2 = me->GetRangeToSqr(threat2->GetLastKnownPosition());

	if (range1 < range2)
	{
		return threat1;
	}

	return threat2;
}