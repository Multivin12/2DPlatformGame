#ifndef NPC_H
#define NPC_H
#include "OBB.h"
#include "GameCharacter.h"
#include <string>
#include <vector>

class NPC : public GameCharacter
{
public:

	NPC();
	void updatePlayerMovement(double dt);
	~NPC();

};
#endif

