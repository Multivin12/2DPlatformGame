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
	Xspeed = 15.0f;
	oldXspeed = 15.0f;
}


/*
 * Method for updating the player's movement when it has collided with something.
 * Also used for updating the texture.
 */
void NPC::updatePlayerMovement(double dt) {

	oldYspeed = Yspeed;
	oldXspeed = Xspeed;


	//update all the X movements
	//if right has been pressed increase the speed up to 2.0
	//equation for updating displacement
	XPla += Xspeed*dt;

	if (XPla > 860.0f || XPla < 0.0f) {
		if (XPla < 0.0f) {
			XPla = 0.0f;
		}
		if (XPla > 860.0f) {
			XPla = 860.0f;
		}
		oldXspeed = -oldXspeed;
		Xspeed = -Xspeed;

		if (textureDirection) {
			textureDirection = false;
		}
		else {
			textureDirection = true;
		}
		
	}


	//Now update the Y movements
	YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;

	//collision handling for the enemy
	if (areCollidingPlatform) {
		for (std::vector<std::string>::iterator it = collisionStatuses.begin();
			it != collisionStatuses.end(); it++) {

			cout << *it << endl;

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

	if (Xspeed < 0) {
		textureDirection = false;
	}
	else {
		textureDirection = true;
	}

	//now update the texture
	if (numFrames % 30 == 0) {
		textureNumber++;
		switch (textureNumber% 4) {

			case 0: loadTexture("Sprites/alien/blue/blue_walk1.png");
					break;

			case 1: loadTexture("Sprites/alien/blue/blue_walk2.png");
					break;

			case 2: loadTexture("Sprites/alien/blue/blue_walk3.png");
					break;

			case 3: loadTexture("Sprites/alien/blue/blue_walk4.png");
					break;
		}
	}



	collisionStatuses.clear();
	numFrames++;
}

NPC::~NPC()
{
}
