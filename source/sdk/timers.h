#ifndef SDK_TIMERS_H_
#define SDK_TIMERS_H_

#include <extdll.h>
#include <meta_api.h>

// Source Engine timers ported to the Goldsrc Engine

/**
 * Simple class for tracking intervals of game time.
 * Upon creation, the timer is invalidated.  To measure time intervals, start the timer via Start().
 */
class IntervalTimer
{
public:
	IntervalTimer(void)
	{
		m_timestamp = -1.0f;
	}

	void Reset(void)
	{
		m_timestamp = Now();
	}

	void Start(void)
	{
		m_timestamp = Now();
	}

	void Invalidate(void)
	{
		m_timestamp = -1.0f;
	}

	bool HasStarted(void) const
	{
		return (m_timestamp > 0.0f);
	}

	/// if not started, elapsed time is very large
	float GetElapsedTime(void) const
	{
		return (HasStarted()) ? (Now() - m_timestamp) : 99999.9f;
	}

	bool IsLessThen(float duration) const
	{
		return (Now() - m_timestamp < duration) ? true : false;
	}

	bool IsGreaterThen(float duration) const
	{
		return (Now() - m_timestamp > duration) ? true : false;
	}

private:
	float m_timestamp;
	float Now(void) const;		// work-around since client header doesn't like inlined gpGlobals->curtime
};

/**
 * Simple class for counting down a short interval of time.
 * Upon creation, the timer is invalidated.  Invalidated countdown timers are considered to have elapsed.
 */
class CountdownTimer
{
public:
	CountdownTimer(void)
	{
		m_timestamp = -1.0f;
		m_duration = 0.0f;
	}

	void Reset(void)
	{
		m_timestamp = Now() + m_duration;
	}

	void Start(float duration)
	{
		m_timestamp = Now() + duration;
		m_duration = duration;
	}

	void Invalidate(void)
	{
		m_timestamp = -1.0f;
	}

	bool HasStarted(void) const
	{
		return (m_timestamp > 0.0f);
	}

	bool IsElapsed(void) const
	{
		return (Now() > m_timestamp);
	}

	float GetElapsedTime(void) const
	{
		return Now() - m_timestamp + m_duration;
	}

	float GetRemainingTime(void) const
	{
		return (m_timestamp - Now());
	}

	/// return original countdown time
	float GetCountdownDuration(void) const
	{
		return (m_timestamp > 0.0f) ? m_duration : 0.0f;
	}

private:
	float m_duration;
	float m_timestamp;
	float Now(void) const;		// work-around since client header doesn't like inlined gpGlobals->curtime
};

inline float IntervalTimer::Now(void) const
{
	return gpGlobals->time;
}

inline float CountdownTimer::Now(void) const
{
	return gpGlobals->time;
}

#endif // !SDK_TIMERS_H_

