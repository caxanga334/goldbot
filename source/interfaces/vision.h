#ifndef VISION_INTERFACE_H_
#define VISION_INTERFACE_H_

#include <algorithm>
#include <vector>

#include "sdk/chandle.h"
#include "memoryentity.h"
#include "component.h"

class IVision : public IBotComponent
{
public:
	IVision(IPluginBot* bot);
	virtual ~IVision();

	virtual void Reset() override;
	virtual void Update() override;
	virtual void UpKeep() override;

	virtual float GetUpdateInterval() override;

	/**
	 * @brief Sets the bot field of view horizontal angle in degrees
	*/
	void SetFieldOfView(float angle);
	float GetFieldOfView() const { return m_fov; }

	virtual int GetMaxSearchEnts() const;

	virtual float GetDefaultFieldOfView() const;
	virtual float GetMaxVisionRange() const;
	// return true to complete ignore this entity
	virtual bool IsIgnored(edict_t* edict) const;
	virtual bool IsInFieldOfView(const Vector& target) const;
	virtual bool IsInFieldOfView(const edict_t* edict) const;

private:
	std::vector<CMemoryEntity> m_knownentities;
	float m_fov; // The bot FOV
	float m_coshalf; // the cosine of FOV/2

	bool IsAwareOf(const CMemoryEntity& other);
	void UpdateVisibleEntities();
	void CollectVisibleEntities();
};

inline float IVision::GetUpdateInterval()
{
	return 0.2f; // vision updates 5 times per second
}

inline float IVision::GetDefaultFieldOfView() const
{
	return 90.0f;
}

inline float IVision::GetMaxVisionRange() const
{
	return 2048.0f;
}

inline bool IVision::IsIgnored(edict_t* edict) const
{
	return false;
}

#endif

