#ifndef NPC_H
#define NPC_H
#include "OBB.h"
#include "GameCharacter.h"
#include "playerCharacter.h"
#include <string>
#include <vector>

class NPC : public GameCharacter
{
public:
	bool isDead = false;
	bool deleteObj = false;

	NPC();
	void updatePlayerMovement(double dt);
	void typeOfCollision(PlayerCharacter &p, double dt);
	~NPC();

};
#endif

