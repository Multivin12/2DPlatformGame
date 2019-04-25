#ifndef Platform_H
#define Platform_H
#include "OBB.h"
#include "playerCharacter.h"
#include <string>
#include <vector>
#include <Windows.h>
#include "Image_Loading/nvImage.h"
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>

class Platform
{
public:
	Point points[NUMVERTS];
	OBB boundingBox;
	GLuint textureID = 0;
	int textureWrapType = 0;


	Platform();
	void loadTexture(char* texturePath);
	GLuint loadPNG(char* name);
	void createPlatformAndDraw(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y);
	void createOBB(float matrix[16]);
	void drawOBB(void);
	virtual std::string typeOfCollision(GameCharacter &p,double dt);
	~Platform();
};
#endif
