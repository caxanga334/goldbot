#ifndef PLAYER_H_
#define PLAYER_H_

#include "sdk/math_vectors.h"
#include "interfaces/player.h"

class IPluginBot;

class CPluginPlayer : public IPlayer
{
public:
	CPluginPlayer(edict_t* edict);
	virtual ~CPluginPlayer();

private:

};

#endif // !PLAYER_H_

