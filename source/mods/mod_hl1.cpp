#include <extdll.h>

#include "mod_base.h"
#include "mod_hl1.h"

CHL1Mod::CHL1Mod() : CBaseMod()
{
}

CHL1Mod::~CHL1Mod()
{
}

const char* CHL1Mod::GetModName()
{
	return "Half-Life 1";
}

const char* CHL1Mod::GetModDataDirectory()
{
	return "valve";
}
