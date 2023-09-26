#ifndef PLUGIN_UTIL_H_
#define PLUGIN_UTIL_H_

#include <vector>
#include <random>

#include "sdk/math_vectors.h"

/**
 * @brief Collects all entities inside the given box
 * @param edictVec Reference to vector where found entities will be stored
 * @param mins Box mins
 * @param maxs Box maxs
*/
void UTIL_CollectEntitiesInABox(std::vector<edict_t*>& edictVec, const Vector& mins, const Vector& maxs);
void UTIL_CollectEntitiesInASphere(std::vector<edict_t*>& edictVec, const Vector& center, const float radius);
void UTIL_DrawBeamFromEnt(int iIndex, Vector vecEnd, int iLifetime, int bRed, int bGreen, int bBlue);
void UTIL_DrawBeamPoints(Vector vecStart, Vector vecEnd, int iLifetime, int bRed, int bGreen, int bBlue);
void CONSOLE_ECHO(char* pszMsg, ...);
void CONSOLE_ECHO_LOGGED(char* pszMsg, ...);
void UTIL_SetBeamSpriteModel(const int modelindex);
edict_t* UTIL_FindEntityInSphere(edict_t* pStartEntity, const Vector& vecCenter, float flRadius);
edict_t* UTIL_FindEntityByString(edict_t* start, const char* keyword, const char* value);
edict_t* UTIL_FindEntityByClassname(edict_t* start, const char* classname);
edict_t* UTIL_FindEntityByTargetname(edict_t* start, const char* targetname);
edict_t* UTIL_FindEntityByTarget(edict_t* start, const char* target);
/**
 * @brief Gets the closest valid player to the start vector
 * @param range The distance to the player will be stored here
*/
edict_t* UTIL_GetClosestPlayer(const Vector& start, const int team, float &range);
edict_t* UTIL_GetClosestPlayer(const Vector* start, const int team, float &range);
edict_t* UTIL_GetListenServerHost();
int UTIL_GetPlayerCount(const int team, edict_t* ignore);
Vector UTIL_ComputeOrigin(entvars_t* pevVars);
Vector UTIL_ComputeOrigin(edict_t* pentEdict);

inline int UTIL_GetRandomInt(const int min, const int max)
{
	std::random_device generator;
	std::uniform_int_distribution<int> dist(min, max);
	return dist(generator);
}

inline float UTIL_GetRandomFloat(const float min, const float max)
{
	std::random_device generator;
	std::uniform_real_distribution<float> dist(min, max);
	return dist(generator);
}

// Random boolean with fixed 50% chance for true
inline bool UTIL_GetRandomBool()
{
	std::random_device generator;
	std::uniform_int_distribution<int> dist(0, 1);
	return dist(generator) == 1;
}

// Random boolean with true chance passed as a param
inline bool UTIL_GetRandomBool(const int chance)
{
	std::random_device generator;
	std::uniform_int_distribution<int> dist(0, 99);
	return dist(generator) < chance;
}

// Random boolean with true chance passed as a param
inline bool UTIL_GetRandomBool(const float chance)
{
	std::random_device generator;
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	return dist(generator) < chance;
}

#endif // !PLUGIN_UTIL_H_

