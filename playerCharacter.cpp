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
	livesLeft = startingLives;
}

/*
 * Method for updating the player's movement when it has collided with something.
 */
void PlayerCharacter::updatePlayerMovement(double dt) {

	
	oldYspeed = Yspeed;
	oldXspeed = Xspeed;

	if (!collidingSpaceship) {
		//update all the X movements
		//if right has been pressed increase the speed 
		if (rightPressed) {
			if (Xspeed < 30.0) {
				//equation for updating speed
				Xspeed = Xspeed + XspeedInc * dt;
				textureDirection = true;
			}
		}


		//same for left pressed
		if (leftPressed) {
			if (Xspeed > -30.0) {
				Xspeed = Xspeed - XspeedInc * dt;
				textureDirection = false;
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
				loadTexture("Sprites/astronautStill.png");
				textureNumber = 0.0;
				numFrames = 0.0;
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
			loadTexture("Sprites/astronautStill.png");
			textureNumber = 0.0;
			numFrames = 0.0;
		}

		//Now update the Y movements
		YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;

		//Stop the player from falling out of the world
		if (YPla < -3000.0) {
			//reset all the values
			YPla = 0.0;
			Yspeed = 0.0;
			oldYspeed = 0.0;
			XPla = 0.0;
			coolDown = true;
			livesLeft--;
		}

		//For processing when the player character should jump
		//the collision status is a string recording where the player character collided from
		if (areCollidingPlatform) {
			for (std::vector<std::string>::iterator it = collisionStatuses.begin();
				it != collisionStatuses.end(); it++) {

				cout << *it << endl;
				if (*it == "side") {
					XPla = XPla - 3.0f*(Xspeed + oldXspeed)*dt;

					Xspeed = 0.0f;
					oldXspeed = 0.0f;
					loadTexture("Sprites/astronautStill.png");
					textureNumber = 0.0;
					numFrames = 0.0;
				}
				else if (*it == "top") {

					if (jumpPressed) {
						jumpCounter = 0;
						loadTexture("Sprites/astronautStill.png");
					}
					//collision response
					Yspeed = 0;
					oldYspeed = 0;
					jumpPressed = false;
				}
				else if (*it == "bottom") {
					Yspeed = -2.0f;
					YPla = YPla + 0.5*(Yspeed + oldYspeed)*dt;
				}
			}
		}
		else {
			cout << "No collision" << endl;
			//if it's not colliding, you want it to be subject to gravity
			Yspeed = oldYspeed - YspeedInc * dt;
		}

		if (coolDown) {

			
			coolDownTime += dt;
			//to determine when the game charater should flash
			if (10 < coolDownTime && coolDownTime < 15 ||
				20 < coolDownTime && coolDownTime < 25 ||
				30 < coolDownTime && coolDownTime < 35 ||
				40 < coolDownTime && coolDownTime < 45 ||
				50 < coolDownTime && coolDownTime < 55 ||
				60 < coolDownTime && coolDownTime < 65 ||
				70 < coolDownTime && coolDownTime < 75 ||
				80 < coolDownTime && coolDownTime < 85 ||
				90 < coolDownTime && coolDownTime < 95) {
				flash = true;
			}
			else {
				flash = false;
			}
			if (numFrames % 10 == 0) {
				glColor3f(0, 0, 0);
			}

			if (coolDownTime >= 100) {
				coolDown = false;
				coolDownTime = 0;
			}
		}

		//Now update the textures
		if (jumpPressed) {
			loadTexture("Sprites/astronautJump.png");
		}
		else {

			int frameGap = (int)round((15.0 / abs(Xspeed))*20.0f);
			//now update the texture
			if (Xspeed == 0.0) {
				//Don't change the texture
			}
			//(round(15.0/Xspeed)*30.0f) this calculation is so that the sprite switches between images
			//when it is moving faster.
			else if (numFrames % frameGap == 0) {

				textureNumber++;
				switch (textureNumber % 9) {

				case 0: loadTexture("Sprites/astronautMove1.png");
					break;

				case 1: loadTexture("Sprites/astronautMove2.png");
					break;

				case 2: loadTexture("Sprites/astronautMove3.png");
					break;

				case 3: loadTexture("Sprites/astronautMove4.png");
					break;

				case 4: loadTexture("Sprites/astronautMove5.png");
					break;

				case 5: loadTexture("Sprites/astronautMove6.png");
					break;

				case 6: loadTexture("Sprites/astronautMove7.png");
					break;

				case 7: loadTexture("Sprites/astronautMove8.png");
					break;

				case 8: loadTexture("Sprites/astronautMove9.png");
					break;
				}
			}
		}

		numFrames++;

		collisionStatuses.clear();
	}
	else {


		Yspeed = oldYspeed + 0.1*YspeedInc * dt;
		YPla = YPla + (Yspeed+oldYspeed)*0.5*dt;
	}
	
}

void PlayerCharacter::resetCharacter() {
	YPla = 0;
	Yspeed = 0.0f;
	YspeedInc = 7.5f;
	oldYspeed = 0.0f;

	XPla = 0.0;
	Xspeed = 0.0;
	XspeedInc = 1.5f;
	oldXspeed = 0.0f;

	textureDirection = true;
	numFrames = 0;
	textureNumber = 0;

	flash = false;
	collidingSpaceship = false;
	areCollidingPlatform = true;

	jumpPressed = false;
	leftPressed = false;
	rightPressed = false;

	//counter to count how long the jump button has been hold down for
	jumpCounter = 0;

	livesLeft = startingLives;

	coolDown = false;
	coolDownTime = 0;

	loadTexture("Sprites/astronautStill.png");
}


PlayerCharacter::~PlayerCharacter()
{
}
