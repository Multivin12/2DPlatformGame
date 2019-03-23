#include "MovingPlatform.h"
#include <iostream>


MovingPlatform::MovingPlatform()
{

}

/*
 * This method is for testing the type of collision that has occured.
 * Uses AABB to do this for now.
 * NOTE: assumes that a collision has previously occured.
*/
std::string MovingPlatform::typeOfCollision(GameCharacter& p,double dt) {

	if (!boundingBox.SAT2D(p.boundingBox)) {
		return "not colliding";
	}

	OBB platform = boundingBox;
	OBB player = p.boundingBox;

	//Test if the max of a platform is less than the player's minimum plus a certain threshold
	if (platform.getMaxY() <= player.getMinY() + 128.0*dt) {
		double difference = player.getMinY() - platform.getMaxY();
		p.YPla -= difference;

		p.XPla += Xspeed * dt;

		p.YPla += 2.0*Yspeed * dt;

		return "top";
	}

	//Test if the min of a platform is greater than the player's minimum minus a certain threshold
	if (platform.getMinY() >= player.getMaxY() - 128.0*dt) {

		double difference = player.getMaxY() - platform.getMinY();
		p.YPla -= difference;

		p.YPla += Yspeed * dt;
		
		
		return "bottom";
	}

	//if you've gotten here it's a side collision
	//need the if statement otherwise it gets stuck on the side
	if (Xspeed < 0) {
		p.XPla += Xspeed * dt;
	}

	if (Xspeed > 0) {
		p.XPla -= Xspeed * dt;
	}
	
	return "side";
}

void MovingPlatform::updatePlatformMovement(double dt) {

	oldYspeed = Yspeed;
	oldXspeed = Xspeed;

	//update all the X movements
	//if right has been pressed increase the speed up to 2.0
	//equation for updating displacement
	XPla += 0.5f*(Xspeed + oldXspeed)*dt;

	if (XPla > 860.0f || XPla < 0.0f) {
		if (XPla < 0.0f) {
			XPla = 0.0f;
		}
		if (XPla > 860.0f) {
			XPla = 860.0f;
		}
		oldXspeed = -oldXspeed;
		Xspeed = -Xspeed;
	}


	//Now update the Y movements
	YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;

	if (YPla > 860.0f || YPla < 0.0f) {
		if (YPla < 0.0f) {
			XPla = 0.0f;
		}
		if (YPla > 860.0f) {
			YPla = 860.0f;
		}
		oldYspeed = -oldYspeed;
		Yspeed = -Yspeed;
	}
}

MovingPlatform::~MovingPlatform()
{
}
