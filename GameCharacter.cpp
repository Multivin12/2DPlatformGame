#include "GameCharacter.h"



GameCharacter::GameCharacter()
{
	this->points[0].x = 0.0;
	this->points[0].y = 0.0;
	this->points[1].x = 0.0;
	this->points[1].y = 0.0;
	this->points[2].x = 0.0;
	this->points[2].y = 0.0;
	this->points[3].x = 0.0;
	this->points[3].x = 0.0;
}

GLuint GameCharacter::loadPNG(char* name)
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);

	return myTextureID;
}

void GameCharacter::addPointsandDraw(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y) {

	this->points[0].x = p1x;
	this->points[0].y = p1y;
	this->points[1].x = p2x;
	this->points[1].y = p2y;
	this->points[2].x = p3x;
	this->points[2].y = p3y;
	this->points[3].x = p4x;
	this->points[3].y = p4y;

	if (flash) {
		glColor3f(0, 0, 0);
	}
	else {
		glColor3f(1, 1, 1);
	}

	if (textureDirection) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_POLYGON);
				glTexCoord2f(0, 0); glVertex2f(points[0].x, points[0].y);
				glTexCoord2f(0, 1); glVertex2f(points[1].x, points[1].y);
				glTexCoord2f(1, 1); glVertex2f(points[2].x, points[2].y);
				glTexCoord2f(1, 0); glVertex2f(points[3].x, points[3].y);
			glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	//to rotate the texture in the opposite direction
	else if(!(textureDirection)) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_POLYGON);
				glTexCoord2f(1, 0); glVertex2f(points[0].x, points[0].y);
				glTexCoord2f(1, 1); glVertex2f(points[1].x, points[1].y);
				glTexCoord2f(0, 1); glVertex2f(points[2].x, points[2].y);
				glTexCoord2f(0, 0); glVertex2f(points[3].x, points[3].y);
			glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}

void GameCharacter::createOBB(float matrix[16]) {
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

void GameCharacter::drawOBB() {
	this->boundingBox.drawOBB();
}

void GameCharacter::loadTexture(char*texturePath) {
	textureID = loadPNG(texturePath);
}

GameCharacter::~GameCharacter()
{
}
