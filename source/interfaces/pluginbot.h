#ifndef PLUGIN_BOT_H_
#define PLUGIN_BOT_H_

#include <iterator>
#include <algorithm>
#include <vector>

#include "sdk/math_vectors.h"
#include "player.h"
#include "eventresponder.h"
#include "component.h"
#include "playerinput.h"
#include "body.h"
#include "vision.h"
#include "intention.h"
#include "locomotion.h"

/**
 * @brief Base plugin bot interface
*/
class IPluginBot : public IPlayer, public IEventResponder
{
public:
	IPluginBot(edict_t* pEdict);
	virtual ~IPluginBot();

	bool BeginUpdate();

	virtual IPluginBot* MyPluginBotPointer() const override;

	/**
	 * @brief Resets the bot to its initial state
	*/
	virtual void Reset();
	/**
	 * @brief Main update function, called at intervals
	*/
	virtual void Update();
	/**
	 * @brief Lightweight function to keep things running, called every frame
	*/
	virtual void UpKeep();

	virtual void RunPlayerMove();

	// overriden from event responder
	virtual IEventResponder* FirstContainedResponder(void) const override;
	virtual IEventResponder* NextContainedResponder(IEventResponder* current) const override;

	virtual void OnSight(CMemoryEntity* them) override;
	virtual void OnLostSight(CMemoryEntity* them) override;

	virtual IBody* GetBodyInterface() const;
	virtual IVision* GetVisionInterface() const;
	virtual IIntention* GetIntentionInterface() const;
	virtual ILocomotion* GetLocomotionInterface() const;

	const float GetRangeTo(edict_t* edict) const;
	const float GetRangeToSqr(edict_t* edict) const;
	const float GetRangeTo(const Vector& other) const;
	const float GetRangeToSqr(const Vector& other) const;

	IPlayerInput* GetInput() { return &m_input; }

protected:
	byte GetUsercmdMsec();
	float m_lastusercmdtime;
	IPlayerInput m_input; // Player button input;

private:
	friend class IBotComponent;
	void RegisterComponent(IBotComponent* component);
	// void UnregisterComponent(IBotComponent* compoenent); // not needed unless we're using std::vector
	// std::vector<IBotComponent*> m_componentList; // yeah, not going to bother
	IBotComponent* m_componentList;

	int m_nextUpdateTick;

	mutable IVision* m_basevision;
	mutable IBody* m_basebody;
	mutable IIntention* m_baseintention;
	mutable ILocomotion* m_baselocomotion;
};

inline const float IPluginBot::GetRangeTo(edict_t* edict) const
{
	return (GetPosition() - edict->v.origin).Length();
}

inline const float IPluginBot::GetRangeToSqr(edict_t* edict) const
{
	return (GetPosition() - edict->v.origin).LengthSqr();
}

inline const float IPluginBot::GetRangeTo(const Vector& other) const
{
	return (GetPosition() - other).Length();
}

inline const float IPluginBot::GetRangeToSqr(const Vector& other) const
{
	return (GetPosition() - other).LengthSqr();
}

#endif // !

