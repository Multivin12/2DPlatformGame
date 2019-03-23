#ifndef GAMECHARACTER_H
#define GAMECHARACTER_H
#include "OBB.h"
#include <vector>

using namespace std;

class GameCharacter
{
public:
	Point points[NUMVERTS];

	double XPla = 0.0;
	double Xspeed = 0.0;
	double XspeedInc = 1.5f;
	double oldXspeed = 0.0f;


	double YPla = 0;
	double Yspeed = 0.0f;
	double YspeedInc = 7.5f;
	double oldYspeed = 0.0f;

	//variable for recording whether the character has collided with a platform or not
	bool areCollidingPlatform = true;
	OBB boundingBox;

	//variable for recording what type of collision the character is making with a platform
	std::vector<std::string> collisionStatuses = {};

	GameCharacter();
	//First 8 coordinates are for the polygon, next 8 are for the texture
	void addPointsandDraw(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y,
		float t1x, float t1y, float t2x, float t2y, float t3x, float t3y, float t4x, float t4y);
	virtual void updatePlayerMovement(double dt) = 0;
	void createOBB(float matrix[16]);
	void drawOBB(void);
	~GameCharacter();
	
};
#endif
