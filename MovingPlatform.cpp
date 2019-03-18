#include "MovingPlatform.h"



MovingPlatform::MovingPlatform()
{

}

/*
 * This method is for testing the type of collision that has occured.
 * Uses AABB to do this for now.
 * NOTE: assumes that a collision has previously occured.
*/
std::string MovingPlatform::typeOfCollision(GameCharacter& p) {

	if (!boundingBox.SAT2D(p.boundingBox)) {
		return "not colliding";
	}

	OBB platform = boundingBox;
	OBB player = p.boundingBox;

	//Test if the max of a platform is less than the player's minimum plus a certain threshold
	if (platform.getMaxY() <= player.getMinY() + 15.0) {
		double difference = player.getMinY() - platform.getMaxY();
		p.YPla -= difference;

		return "top";
	}

	//Test if the min of a platform is greater than the player's minimum minus a certain threshold
	if (platform.getMinY() >= player.getMaxY() - 15.0) {

		double difference = player.getMaxY() - platform.getMinY();
		p.YPla -= difference;

		return "bottom";
	}

	return "side";
}


MovingPlatform::~MovingPlatform()
{
}
