#include "OBB.h"
#include "GameCharacter.h"
#include <string>
#include <vector>
class Spaceship: public GameCharacter
{
public:

	bool isCollidingPlayer = false;


	Spaceship();
	void resetCharacter();
	void updatePlayerMovement(double dt);
	~Spaceship();
};
