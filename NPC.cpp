#include "NPC.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>


/*
 * Constructor for this class, builds the polygon by invoking the GameCharacter() super class.
*/
NPC::NPC(float Xspeed, string colour, float distanceTravelled,int lives,int damageInflicted,bool jumping, float jumpSpeed):GameCharacter()
{
	this->Xspeed = Xspeed;
	oldXspeed = Xspeed;
	this->colour = colour;
	this->distanceTravelled = distanceTravelled;
	this->damageInflicted = damageInflicted;
	this->jumping = jumping;
	this->jumpSpeed = jumpSpeed;
	startingLives = lives;

	originalXspeed = Xspeed;
	livesLeft = startingLives;
}

/*
   Method for determining the type of collision the player has made with this NPC.
*/
void NPC::typeOfCollision(PlayerCharacter &p, double dt) {

	if (!isDead) {
		OBB npc = boundingBox;
		OBB player = p.boundingBox;

		//top side
		if (!p.coolDown) {
			if (npc.getMaxY() <= player.getMinY() + 64.0*dt) {
				p.Yspeed = 40;
				textureNumber = 0;
				livesLeft--;
				if (livesLeft == 0) {
					isDead = true;
				}
			}
			//bottom side
			else if (npc.getMinY() >= player.getMaxY() - 32.0*dt) {
				p.Yspeed = -0.5f*p.Yspeed;
				p.livesLeft -= damageInflicted;
				p.coolDown = true;
			}
			else {
				//right side
				p.Yspeed = 70.0f;
				if (npc.getMaxX() <= player.getMinX() + 32.0*dt) {
					p.Xspeed = Xspeed * 2.0;
				}
				//left side
				else if (npc.getMinX() >= player.getMaxX() - 32.0*dt) {
					p.Xspeed = Xspeed * 2.0;
				}
				p.livesLeft -= damageInflicted;
				p.coolDown = true;
			}
		}
		else {
			if (npc.getMaxY() <= player.getMinY() + 128.0*dt) {
				p.Yspeed = -p.Yspeed;
				textureNumber = 0;
				livesLeft--;
				if (livesLeft == 0) {
					isDead = true;
				}
			}
			//bottom side
			else if (npc.getMinY() >= player.getMaxY() - 48.0*dt) {

			}
			else {
			}
		}
	}
}


/*
 * Method for updating the player's movement when it has collided with something.
 * Also used for updating the texture.
 */
void NPC::updatePlayerMovement(double dt) {

	oldYspeed = Yspeed;
	oldXspeed = Xspeed;

	if (deleteObj) {
		XPla = -99999999;
		YPla = -99999999;
	} else if (!isDead) {
		//update all the X movements
		//if right has been pressed increase the speed up to 2.0
		//equation for updating displacement
		XPla += Xspeed * dt;

		if (originalXspeed > 0) {
			if (XPla > distanceTravelled || XPla < 0.0f) {
				if (XPla < 0.0f) {
					XPla = 0.0f;
				}
				if (XPla > distanceTravelled) {
					XPla = distanceTravelled;
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
		}
		else {
			if (XPla < -distanceTravelled || XPla > 0.0f) {
				if (XPla > 0.0f) {
					XPla = 0.0f;
				}
				if (XPla < -distanceTravelled) {
					XPla = -distanceTravelled;
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
		}

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
					jumpPressed = false;
					Yspeed = 0.0;
					oldYspeed = 0.0;
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
		if (numFrames % ((int) round(600.0f/Xspeed)) == 0) {
			textureNumber++;
			switch (textureNumber % 4) {

			case 0: loadTexture("Sprites/alien/" + colour + "/" + colour + "_walk1.png");
				break;

			case 1: loadTexture("Sprites/alien/" + colour + "/" + colour + "_walk2.png");
				break;

			case 2: loadTexture("Sprites/alien/" + colour + "/" + colour + "_walk3.png");
				break;

			case 3: loadTexture("Sprites/alien/" + colour + "/" + colour + "_walk4.png");
				break;
			}
		}
	}
	else {
		//run when the alien is dying
		if (Xspeed < 0) {
			textureDirection = false;
		}
		else {
			textureDirection = true;
		}

		//now update the texture
		if (numFrames % 100 == 0) {
			textureNumber++;
			switch (textureNumber % 10) {

			case 0: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead1.png");
				break;

			case 1: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead2.png");
				break;

			case 2: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead3.png");
				break;

			case 3: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead4.png");
				break;

			case 4: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead5.png");
				break;

			case 5: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead6.png");
				break;

			case 6: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead7.png");
				break;

			case 7: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead8.png");
				break;

			case 8: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead9.png");
				break;

			case 9: loadTexture("Sprites/alien/" + colour + "/" + colour + "_dead10.png");
				deleteObj = true;
				break;
			}
		}
	}

	collisionStatuses.clear();
	numFrames++;
}

void NPC::resetCharacter() {
	YPla = 0;
	YspeedInc = 7.5f;
	jumpPressed = false;

	XPla = 0.0;
	XspeedInc = 1.5f;

	textureDirection = true;
	numFrames = 0;
	textureNumber = 0;

	flash = false;
	collidingSpaceship = false;
	areCollidingPlatform = true;

	isDead = false;
	deleteObj = false;

	livesLeft = startingLives;

}

NPC::~NPC()
{
}
