#include "GameCharacter.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLu.h>
#include <iostream>


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

void GameCharacter::addPointsandDraw(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y, float p4x, float p4y,
	float t1x, float t1y, float t2x, float t2y, float t3x, float t3y, float t4x, float t4y) {

	this->points[0].x = p1x;
	this->points[0].y = p1y;
	this->points[1].x = p2x;
	this->points[1].y = p2y;
	this->points[2].x = p3x;
	this->points[2].y = p3y;
	this->points[3].x = p4x;
	this->points[3].y = p4y;

	glBegin(GL_POLYGON);
	for (int i = 0; i < NUMVERTS; i++) {
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();
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

GameCharacter::~GameCharacter()
{
}
