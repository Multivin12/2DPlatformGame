#include "playerCharacter.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>


/*
 * Constructor for this class, builds the polygon
*/
PlayerCharacter::PlayerCharacter(): GameCharacter()
{
}

/*
 * Method for updating the player's movement when it has collided with something.
 */
void PlayerCharacter::updatePlayerMovement(double dt) {

	oldYspeed = Yspeed;
	oldXspeed = Xspeed;

	//update all the X movements
	//if right has been pressed increase the speed up to 2.0
	if (rightPressed) {
		if (Xspeed < 20.0) {
			//equation for updating speed
			Xspeed = Xspeed+ XspeedInc*dt;
		}
	} 

	//same for left pressed
	if (leftPressed) {
		if (Xspeed > -20.0) {
			Xspeed = Xspeed - XspeedInc*dt;
		}
	}


	//For slowing down the player if left or right button isn't pressed 
	//so that the block doesn't keep moving at a constant speed
	if (!(rightPressed) && !(leftPressed) && (abs(Xspeed) > XspeedInc)) {
		if (Xspeed > XspeedInc) {
			Xspeed -= XspeedInc * dt;
		}
		else {
			Xspeed += XspeedInc * dt;
		}
		
		if (abs(Xspeed) < XspeedInc) {
			oldXspeed = 0.0;
			Xspeed = 0.0;
		}
	}

	//equation for updating displacement
	XPla = XPla + 0.5f*(Xspeed + oldXspeed)*dt;
	
	//Stop the player from crossing the start of the world
	if (XPla < 0.0) {
		//reset all the values
		XPla = 0.0;
		Xspeed = 0.0;
		oldXspeed = 0.0;
	}

	//Now update the Y movements
	YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;

	//Stop the player from falling out of the world
	if (YPla < 0.0) {
		//reset all the values
		YPla = 0.0;
		Yspeed = 0.0;
		oldYspeed = 0.0;
	}

	//For processing when the player character should jump
	//the collision status is a string recording where the player character collided from
	if (areCollidingPlatform) {
		for (std::vector<std::string>::iterator it = collisionStatuses.begin();
			it != collisionStatuses.end(); it++) {

			if (*it == "side") {
				XPla = XPla - 3.0f*(Xspeed + oldXspeed)*dt;

				Xspeed = 0.0f;
				oldXspeed = 0.0f;
			}
			else if (*it == "top") {
					
				//collision response
				Yspeed = 0;
				oldYspeed = 0;
				jumpPressed = false;
				jumpCounter = 0;
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

PlayerCharacter::~PlayerCharacter()
{
}
