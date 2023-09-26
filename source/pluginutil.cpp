#include <extdll.h>
#include <meta_api.h>
#include "pluginutil.h"

static int s_beamsprite;

void UTIL_CollectEntitiesInABox(std::vector<edict_t*>& edictVec, const Vector& mins, const Vector& maxs)
{
	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/util.cpp#L404

	edict_t* edict = g_engfuncs.pfnPEntityOfEntIndex(1);
	Vector origin = vec3_origin;

	if (!edict)
		return;

	for (int i = 1; i < gpGlobals->maxEntities; i++, edict++)
	{
		if (edict->free)
			continue;

		origin = Vector(edict->v.origin);

		if (!origin.WithinAABox(mins, maxs))
			continue;

		if (edict->pvPrivateData == nullptr)
			continue;

		edictVec.push_back(edict);
	}
}

void UTIL_CollectEntitiesInASphere(std::vector<edict_t*>& edictVec, const Vector& center, const float radius)
{
	edict_t* edict = g_engfuncs.pfnPEntityOfEntIndex(1);
	Vector origin = vec3_origin;

	if (!edict)
		return;

	for (int i = 1; i < gpGlobals->maxEntities; i++, edict++)
	{
		if (edict->free)
			continue;

		origin = Vector(edict->v.origin);

		if (center.DistToSqr(origin) > radius * radius)
			continue;

		if (edict->pvPrivateData == nullptr)
			continue;

		edictVec.push_back(edict);
	}
}

//------------------------------------------------------------------------------------------------------------
void UTIL_DrawBeamFromEnt(int iIndex, Vector vecEnd, int iLifetime, int bRed, int bGreen, int bBlue)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecEnd);   // vecEnd = origin???
	WRITE_BYTE(TE_BEAMENTPOINT);
	WRITE_SHORT(iIndex);
	WRITE_COORD(vecEnd.x);
	WRITE_COORD(vecEnd.y);
	WRITE_COORD(vecEnd.z);
	WRITE_SHORT(s_beamsprite);
	WRITE_BYTE(0);		 // startframe
	WRITE_BYTE(0);		 // framerate
	WRITE_BYTE(iLifetime); // life
	WRITE_BYTE(10);		 // width
	WRITE_BYTE(0);		 // noise
	WRITE_BYTE(bRed);		 // r, g, b
	WRITE_BYTE(bGreen);		 // r, g, b
	WRITE_BYTE(bBlue);    // r, g, b
	WRITE_BYTE(255);	 // brightness
	WRITE_BYTE(0);		 // speed
	MESSAGE_END();
}


//------------------------------------------------------------------------------------------------------------
void UTIL_DrawBeamPoints(Vector vecStart, Vector vecEnd, int iLifetime, int bRed, int bGreen, int bBlue)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecStart);
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(vecStart.x);
	WRITE_COORD(vecStart.y);
	WRITE_COORD(vecStart.z);
	WRITE_COORD(vecEnd.x);
	WRITE_COORD(vecEnd.y);
	WRITE_COORD(vecEnd.z);
	WRITE_SHORT(s_beamsprite);
	WRITE_BYTE(0);		 // startframe
	WRITE_BYTE(0);		 // framerate
	WRITE_BYTE(iLifetime); // life
	WRITE_BYTE(10);		 // width
	WRITE_BYTE(0);		 // noise
	WRITE_BYTE(bRed);		 // r, g, b
	WRITE_BYTE(bGreen);		 // r, g, b
	WRITE_BYTE(bBlue);    // r, g, b
	WRITE_BYTE(255);	 // brightness
	WRITE_BYTE(0);		 // speed
	MESSAGE_END();
}


//------------------------------------------------------------------------------------------------------------
void CONSOLE_ECHO(char* pszMsg, ...)
{
	va_list     argptr;
	static char szStr[1024];

	va_start(argptr, pszMsg);
	vsprintf(szStr, pszMsg, argptr);
	va_end(argptr);

	(*g_engfuncs.pfnServerPrint)(szStr);
}


//------------------------------------------------------------------------------------------------------------
void CONSOLE_ECHO_LOGGED(char* pszMsg, ...)
{
	va_list     argptr;
	static char szStr[1024];

	va_start(argptr, pszMsg);
	vsprintf(szStr, pszMsg, argptr);
	va_end(argptr);

	(*g_engfuncs.pfnServerPrint)(szStr);
	UTIL_LogPrintf(szStr);
}

void UTIL_SetBeamSpriteModel(const int modelindex)
{
	s_beamsprite = modelindex;
}


edict_t* UTIL_FindEntityInSphere(edict_t* pStartEntity, const Vector& vecCenter, float flRadius)
{
	edict_t* pentEntity = nullptr;

	pentEntity = FIND_ENTITY_IN_SPHERE(pStartEntity, vecCenter, flRadius);

	if (!FNullEnt(pentEntity) && pentEntity->pvPrivateData != nullptr)
		return pentEntity;

	return nullptr;
}

edict_t* UTIL_FindEntityByString(edict_t* start, const char* keyword, const char* value)
{
	edict_t* pentEntity = nullptr;
	pentEntity = FIND_ENTITY_BY_STRING(start, keyword, value);

	if (!FNullEnt(pentEntity))
		return pentEntity;
	return nullptr;
}

edict_t* UTIL_FindEntityByClassname(edict_t* start, const char* classname)
{
	return UTIL_FindEntityByString(start, "classname", classname);
}

edict_t* UTIL_FindEntityByTargetname(edict_t* start, const char* targetname)
{
	return UTIL_FindEntityByString(start, "targetname", targetname);
}

edict_t* UTIL_FindEntityByTarget(edict_t* start, const char* target)
{
	return UTIL_FindEntityByString(start, "target", target);
}

edict_t* UTIL_GetClosestPlayer(const Vector& start, const int team, float& range)
{
	edict_t* result = nullptr;
	float distance = 0.0f;
	float shorest = 999999999.0f;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		auto player = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!player)
			continue;

		if (FNullEnt(player))
			continue;

		if (player->pvPrivateData == nullptr)
			continue;

		if (team > 0 && player->v.team != team)
			continue;

		auto& origin = player->v.origin;
		distance = origin.DistTo(start);

		if (distance < shorest)
		{
			shorest = distance;
			range = shorest;
			result = player;
		}
	}

	return result;
}

edict_t* UTIL_GetClosestPlayer(const Vector* start, const int team, float& range)
{
	edict_t* result = nullptr;
	float distance = 0.0f;
	float shorest = 999999999.0f;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		auto player = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!player)
			continue;

		if (FNullEnt(player))
			continue;

		if (player->pvPrivateData == nullptr)
			continue;

		if (team > 0 && player->v.team != team)
			continue;

		auto& origin = player->v.origin;
		distance = start->DistTo(origin);

		if (distance < shorest)
		{
			shorest = distance;
			range = shorest;
			result = player;
		}
	}

	return result;
}

edict_t* UTIL_GetListenServerHost()
{
	if (IS_DEDICATED_SERVER() > 0)
		return nullptr;

	return g_engfuncs.pfnPEntityOfEntIndex(1);
}

int UTIL_GetPlayerCount(const int team, edict_t* ignore)
{
	int result = 0;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		auto player = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!player)
			continue;

		if (FNullEnt(player))
			continue;

		if (player->pvPrivateData == nullptr)
			continue;

		if (team > 0 && player->v.team != team)
			continue;

		result++;
	}

	return result;
}

Vector UTIL_ComputeOrigin(entvars_t* pevVars)
{
	if ((pevVars->origin.x == 0.0f) && (pevVars->origin.y == 0.0f) && (pevVars->origin.z == 0.0f))
		return (pevVars->absmax + pevVars->absmin) * 0.5f;
	else
		return pevVars->origin;
}

Vector UTIL_ComputeOrigin(edict_t* pentEdict)
{
	return UTIL_ComputeOrigin(VARS(pentEdict));
}
