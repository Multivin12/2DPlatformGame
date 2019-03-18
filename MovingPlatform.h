#pragma once
#include "Platform.h"
class MovingPlatform : public Platform
{
public:

	double XPla = 0.0;
	double Xspeed = 0.0;
	double XspeedInc = 1.0f;
	double oldXspeed = 0.0f;


	double YPla = 0;
	double Yspeed = 0.0f;
	double YspeedInc = 6.0f;
	double oldYspeed = 0.0f;

	MovingPlatform();
	string typeOfCollision(GameCharacter& p);
	~MovingPlatform();
};

