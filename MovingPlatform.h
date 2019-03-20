#pragma once
#include "Platform.h"
class MovingPlatform : public Platform
{
public:

	double XPla = 0.0;
	double Xspeed = 0.0f;
	double oldXspeed = 0.0f;


	double YPla = 0;
	double Yspeed = 10.0f;
	double oldYspeed = 10.0f;

	MovingPlatform();
	string typeOfCollision(GameCharacter& p,double dt);
	void updatePlatformMovement(double dt);
	~MovingPlatform();
};

