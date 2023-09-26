#ifndef MEMORY_ENTITY_H_
#define MEMORY_ENTITY_H_

#include "sdk/chandle.h"
#include "sdk/math_vectors.h"

class CWaypoint;

/**
 * @brief Memory entity - Represents a single entity/edict memorized by the bot
*/
class CMemoryEntity
{
public:
	CMemoryEntity(edict_t* edict);
	virtual ~CMemoryEntity() {}

	void UpdatePosition();

	void UpdateVisibleStatus(const bool visible);

	bool IsObsolete() const;

	edict_t* GetEdict() const { return m_handle.Get(); }

	bool WasLastKnownPositionSeen() const { return m_lpkwasseen; }

	bool IsVisibleRightNow() const { return m_visible; }

	bool WasVisibleRecently() const;
	// Returns the time since this memorized entity was last updated
	float GetTimeSinceLastSeen() const;
	// Returns the time since this memorized entity was memorized
	float GetTimeSinceMemorized() const;

	CWaypoint* GetLastKnownWaypoint() const { return m_lastknownwaypoint; }

	const Vector& GetLastKnownPosition() const { return m_lastknownposition; }

	const Vector& GetLastKnownVelocity() const { return m_lastknownvelocity; }

	bool operator==(const CMemoryEntity& other) const;
	void operator=(const CMemoryEntity& other);

private:
	CHandle m_handle;
	CWaypoint* m_lastknownwaypoint; // Nearest waypoint of the LKP
	Vector m_lastknownposition; // Last known position vector
	Vector m_lastknownvelocity; // Velocity of the entity at the time of the last update
	float m_timesinceknown; // Time since this entity was memorized
	float m_timesincelastseen; // Time since this entity was last seen/updated
	bool m_visible; // Flagged as visible by the vision system
	bool m_lpkwasseen; // Was the LKP seen by the bot?
};

#endif // !MEMORY_ENTITY_H_



#include "sdk/chandle.h"
