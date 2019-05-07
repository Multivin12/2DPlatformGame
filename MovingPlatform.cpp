#include "MovingPlatform.h"
#include <iostream>


MovingPlatform::MovingPlatform(double Xspeed, double distanceTravelledX, double Yspeed, double distanceTravelledY)
{
	this->Xspeed = Xspeed;
	this->distanceTravelledX = distanceTravelledX;
	this->Yspeed = Yspeed;
	this->distanceTravelledY = distanceTravelledY;
	this->originalXspeed = Xspeed;
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

		if (Xspeed != 0.0) {
			p.XPla += Xspeed*dt;
		}
		
		return "top";
	}

	//Test if the min of a platform is greater than the player's minimum minus a certain threshold
	if (platform.getMinY() >= player.getMaxY() - 64.0*dt) {

		double difference = player.getMaxY() - platform.getMinY();
		p.YPla -= difference;
		
		return "bottom";
	}

	//if you've gotten here it's a side collision
	//need the if statement otherwise it gets stuck on the side
	if (Xspeed > 0) {
		p.oldXspeed = -Xspeed;
		p.Xspeed = -Xspeed;
	}
	else {
		p.oldXspeed = -Xspeed*2.0;
		p.Xspeed = -Xspeed*2.0;
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

	if (originalXspeed > 0) {
		if (XPla > distanceTravelledX || XPla < 0.0f) {
			if (XPla < 0.0f) {
				XPla = 0.0f;
			}
			if (XPla > distanceTravelledX) {
				XPla = distanceTravelledX;
			}
			oldXspeed = -oldXspeed;
			Xspeed = -Xspeed;
		}
	}
	else {
		if (XPla < -distanceTravelledX || XPla > 0.0f) {
			if (XPla > 0.0f) {
				XPla = 0.0f;
			}
			if (XPla < -distanceTravelledX) {
				XPla = -distanceTravelledX;
			}
			oldXspeed = -oldXspeed;
			Xspeed = -Xspeed;
		}
	}
	


	//Now update the Y movements
	YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;

	if (YPla > distanceTravelledY || YPla < 0.0f) {
		if (YPla < 0.0f) {
			YPla = 0.0f;
		}
		if (YPla > distanceTravelledY) {
			YPla = distanceTravelledY;
		}
		oldYspeed = -oldYspeed;
		Yspeed = -Yspeed;
	}
}

MovingPlatform::~MovingPlatform()
{
}
