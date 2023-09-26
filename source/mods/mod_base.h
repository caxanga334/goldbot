#ifndef MOD_BASE_H_
#define MOD_BASE_H_

#include "sdk/chandle.h"

class IPluginBot;
class CBaseBot;

namespace Mods
{
	enum ID
	{
		Game_Unknown = 0,
		Game_HL1, // Half-Life 1 multiplayer
		Game_OP4, // Opposing-Force
		Game_Svencoop, // Sven coop

		Game_MAX // total number of known games
	};

	extern void IdentifyGame(const char* gamefolder);
}

/**
 * @brief Base game mod implementation
*/
class CBaseMod
{
public:
	CBaseMod();
	virtual ~CBaseMod();

	// Called every server frame
	virtual void Update() {};
	// Gets the mod name for logging purposes
	virtual const char* GetModName() { return "CBaseMod"; }
	// Called when a client joins the server
	virtual void OnClientPutInServer(edict_t* client) { return; }
	// Called when a client disconnects from the server
	virtual void OnClientDisconnect(edict_t* client) { return; }
	// For round base mods, called when a new round starts
	virtual void OnRoundRestart() { return; }
	// Called when a level change is detected
	virtual void OnChangeLevel() { return; }
	// Is a free for all PvP mod?
	virtual bool IsFreeForAll() { return true; }
	// Is a team based PvP mod?
	virtual bool IsTeamGame() { return false; }
	// Is a PvE mod?
	virtual bool IsCoop() { return false; }
	// Counts the number of clients (players) in game, this function is not very realiable
	virtual int GetClientCount(bool includeBots = true);
	// Creates and adds a bot to the game
	virtual IPluginBot* CreateAndAddBot();
	// Gets the entity classname for the player entity
	virtual const char* GetPlayerEntityClassname() { return "player"; }
	// Directory name where data will be saved
	virtual const char* GetModDataDirectory() { return "cbasemod"; }
	// Vanilla HL, the player origin isn't at the player feet unlike the source engine, 
	// this returns the offset to get the ground position from a player origin vector
	virtual float GetPlayerOriginGroundOffset() { return 34; }
	// Same as above but for crouched players
	virtual float GetCrouchedPlayerOriginGroundOffset() { return 17; }

protected:
	// Creates the mod specific bot class
	virtual CBaseBot* AllocateBot(edict_t* edict);

private:
};

extern Mods::ID g_currentMod;
extern CBaseMod* GameModFactory();
extern CBaseMod* gamemod;


#endif // !MOD_BASE_H_

