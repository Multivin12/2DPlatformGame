#include "Platform.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>



Platform::Platform()
{
}

void Platform::createPlatformAndDraw(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y) {

	this->points[0].x = p1x;
	this->points[0].y = p1y;
	this->points[1].x = p2x;
	this->points[1].y = p2y;
	this->points[2].x = p3x;
	this->points[2].y = p3y;
	this->points[3].x = p4x;
	this->points[3].y = p4y;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POLYGON);
			if (textureWrapType == 0) {
				glTexCoord2f(0, 0); glVertex2f(points[0].x, points[0].y);
				glTexCoord2f(0, 1); glVertex2f(points[1].x, points[1].y);
				glTexCoord2f(20000.0/128.0, 1); glVertex2f(points[2].x, points[2].y);
				glTexCoord2f(20000.0/128.0, 0); glVertex2f(points[3].x, points[3].y);
			}
			else if(textureWrapType ==1 ) {
				glTexCoord2f(0, 0); glVertex2f(points[0].x, points[0].y);
				glTexCoord2f(0, 1); glVertex2f(points[1].x, points[1].y);
				glTexCoord2f(1, 1); glVertex2f(points[2].x, points[2].y);
				glTexCoord2f(1, 0); glVertex2f(points[3].x, points[3].y);
			}
			else {

			}
		glEnd();
	glDisable(GL_TEXTURE_2D);
}

void Platform::createOBB(float matrix[16]) {

	boundingBox = OBB();

	//get the necessary points for the OBB
	this->boundingBox.vertOriginal[0].x = points[0].x;
	this->boundingBox.vertOriginal[0].y = points[0].y;
	this->boundingBox.vertOriginal[1].x = points[1].x;
	this->boundingBox.vertOriginal[1].y = points[1].y;
	this->boundingBox.vertOriginal[2].x = points[2].x;
	this->boundingBox.vertOriginal[2].y = points[2].y;
	this->boundingBox.vertOriginal[3].x = points[3].x;
	this->boundingBox.vertOriginal[3].y = points[3].y;

	this->boundingBox.transformPoints(matrix);
}

void Platform::drawOBB(void) {
	this->boundingBox.drawOBB();
}

/*
 * This method is for testing the type of collision that has occured.
 * Uses AABB to do this for now.
 * NOTE: assumes that a collision has previously occured.
*/
std::string Platform::typeOfCollision(GameCharacter& p,double dt) {


	if (!boundingBox.SAT2D(p.boundingBox)) {
		return "not colliding";
	}

	OBB platform = boundingBox;
	OBB player = p.boundingBox;


	//Test if the max of a platform is less than the player's minimum plus a certain threshold
	//the threshold is just to compensate for overlap as the object might not necessarily collide perfectly.
												//128*dt works quite well
	if (platform.getMaxY() <= player.getMinY()+128.0*dt) {
		double difference = player.getMinY() - platform.getMaxY();
		p.YPla -= difference;

		return "top";
	}
		
	//Test if the min of a platform is greater than the player's minimum minus a certain threshold
	if (platform.getMinY() >= player.getMaxY() - 64.0*dt) {

		double difference = player.getMaxY() - platform.getMinY();
		p.YPla -= difference;

		return "bottom";
	}

	return "side";
}

GLuint Platform::loadPNG(char* name)
{
	// Texture loading object
	nv::Image img;

	GLuint myTextureID;

	// Return true on success
	if (img.loadImageFromFile(name))
	{
		glGenTextures(1, &myTextureID);
		glBindTexture(GL_TEXTURE_2D, myTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, img.getInternalFormat(), img.getWidth(), img.getHeight(), 0, img.getFormat(), img.getType(), img.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);

	return myTextureID;
}

void Platform::loadTexture(char*texturePath) {
	textureID = loadPNG(texturePath);
}

Platform::~Platform()
{
}
