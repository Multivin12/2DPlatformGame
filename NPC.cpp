#include "NPC.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>


/*
 * Constructor for this class, builds the polygon by invoking the GameCharacter() super class.
*/
NPC::NPC():GameCharacter()
{
	Xspeed = 8.0f;
	oldXspeed = 8.0f;
}


/*
 * Method for updating the player's movement when it has collided with something.
 */
void NPC::updatePlayerMovement(double dt) {

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

	cout << Xspeed << endl;


	//Now update the Y movements
	YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;

	//collision handling for the enemy
	if (areCollidingPlatform) {
		for (std::vector<std::string>::iterator it = collisionStatuses.begin();
			it != collisionStatuses.end(); it++) {


			if (*it == "side") {
				XPla = XPla - 3.0f*(Xspeed + oldXspeed)*dt;

				Xspeed = -Xspeed;
				oldXspeed = -oldXspeed;
			}
			else if (*it == "top") {

				//collision response
				Yspeed = 0;
				oldYspeed = 0;
			}
			else if (*it == "bottom") {
				Yspeed = -2.0f;
				YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;
			}
		}
	}
	else {
		//if it's not colliding, you want it to be subject to gravity
		Yspeed = oldYspeed - YspeedInc * dt;
	}

	collisionStatuses.clear();
}

NPC::~NPC()
{
}
