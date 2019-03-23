#pragma once
#include "Platform.h"
class MovingPlatform : public Platform
{
public:

	double XPla = 0.0;
	double Xspeed = 10.0f;
	double oldXspeed = 10.0f;


	double YPla = 0;
	double Yspeed = 0.0f;
	double oldYspeed = 0.0f;

	MovingPlatform();
	string typeOfCollision(GameCharacter& p,double dt);
	void updatePlatformMovement(double dt);
	~MovingPlatform();
};

