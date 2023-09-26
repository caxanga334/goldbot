#ifndef MANAGER_H_
#define MANAGER_H_

#include <algorithm>
#include <vector>
#include <cstring>

class IPlayer;
class IPluginBot;
class CPluginPlayer;

class CPluginBotManager
{
public:
	CPluginBotManager();
	virtual ~CPluginBotManager();

	void Update();

	/**
	 * @brief Creates the folders where plugin data is stored
	*/
	void CreatePluginDirectories(const char* gamedir);

	CPluginPlayer* AllocateNewPlayer(edict_t* edict);
	void OnClientPutInServer(edict_t* edict);

	void RegisterBot(IPluginBot* bot);
	void RegisterPlayer(IPlayer* player);
	void UnregisterBot(IPluginBot* bot);
	void UnregisterPlayer(IPlayer* player);

	bool ClientCommand(IPlayer* player, const char* command, const std::vector<const char*>& args, const int argc);

	IPlayer* GetPlayerOfEdict(edict_t* edict);
	IPluginBot* GetBotOfEdict(edict_t* edict);
private:
	std::vector<IPluginBot*> m_botlist;
	std::vector<IPlayer*> m_playerlist;
};

extern CPluginBotManager* TheBotManager;

#endif // !MANAGER_H_

