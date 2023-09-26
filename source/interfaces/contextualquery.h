#ifndef CONTEXTUAL_QUERY_H_
#define CONTEXTUAL_QUERY_H_

#include "sdk/math_vectors.h"

class IPluginBot;
class CMemoryEntity;
class CWaypoint;

enum QueryResultType
{
	ANSWER_NO,
	ANSWER_YES,
	ANSWER_UNDEFINED
};

class IContextualQuery
{
public:
	virtual ~IContextualQuery();

	// Should the bot pick up the given item?
	virtual QueryResultType ShouldPickUp(const IPluginBot* me, edict_t* item) const;
	// Should the bot attack the given entity?
	virtual QueryResultType ShouldAttack(const IPluginBot* me, const CMemoryEntity* them) const;
	// Is the bot in a hurry?
	virtual QueryResultType ShouldHurry(const IPluginBot* me) const;
	// Should the bot retreat?
	virtual QueryResultType ShouldRetreat(const IPluginBot* me) const;
	// Should the bot roam around the map?
	virtual QueryResultType ShouldRoam(const IPluginBot* me) const;
	// Should the bot use this item/object?
	virtual QueryResultType ShouldUse(const IPluginBot* me, edict_t* entity) const;
	// Should the bot wait for the blocker entity that is in the bot's path?
	virtual QueryResultType IsHindrance(const IPluginBot* me, edict_t* blocker) const;
	// Given a subject, return the point where the bot should aim at
	virtual Vector SelectTargetPoint(const IPluginBot* me, edict_t* subject) const;
	// Given two threats, select which is more dangerous to the bot
	virtual const CMemoryEntity* SelectMoreDangerousThreat(const IPluginBot* me, const CMemoryEntity* threat1, const CMemoryEntity* threat2) const;
};

inline QueryResultType IContextualQuery::ShouldPickUp(const IPluginBot* me, edict_t* item) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldAttack(const IPluginBot* me, const CMemoryEntity* them) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldHurry(const IPluginBot* me) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldRetreat(const IPluginBot* me) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldRoam(const IPluginBot* me) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldUse(const IPluginBot* me, edict_t* entity) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::IsHindrance(const IPluginBot* me, edict_t* blocker) const
{
	return ANSWER_UNDEFINED;
}

inline Vector IContextualQuery::SelectTargetPoint(const IPluginBot* me, edict_t* subject) const
{
	return vec3_origin;
}

inline const CMemoryEntity* IContextualQuery::SelectMoreDangerousThreat(const IPluginBot* me, const CMemoryEntity* threat1, const CMemoryEntity* threat2) const
{
	return nullptr;
}

#endif // !CONTEXTUAL_QUERY_H_

