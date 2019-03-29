#ifndef playerCharacter_H
#define playerCharacter_H
#include "OBB.h"
#include "GameCharacter.h"
#include <string>
#include <vector>

class PlayerCharacter: public GameCharacter
{
public:

	//variables to control when a button has been pressed or not
	bool jumpPressed = false;
	bool leftPressed = false;
	bool rightPressed = false;

	//counter to count how long the jump button has been hold down for
	int jumpCounter = 0;

	int livesLeft = 3;

	bool coolDown = false;
	int coolDownTime = 0;

	PlayerCharacter();
	void updatePlayerMovement(double dt);
	~PlayerCharacter();
};

#endif

