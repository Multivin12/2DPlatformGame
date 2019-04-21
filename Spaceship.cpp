#include "Spaceship.h"
#include "playerCharacter.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>


Spaceship::Spaceship(): GameCharacter()
{
}

void Spaceship::updatePlayerMovement(double dt) {
	if (isCollidingPlayer) {
		oldYspeed = Yspeed;
		oldXspeed = Xspeed;

		Yspeed = oldYspeed + 0.1*YspeedInc * dt;
		YPla = YPla + (Yspeed + oldYspeed)*0.5*dt;
	}
}

void Spaceship::resetCharacter() {
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

	isCollidingPlayer = false;
}

Spaceship::~Spaceship()
{
}
