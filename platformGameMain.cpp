
//includes areas for keyboard control, mouse control, resizing the window
//and draws a spinning rectangle

#include <windows.h>		// Header file for Windows
#include "irrKlang-1.6.0/include/irrKlang.h"
#include <iostream>
#include <fstream>
#include <string>			// used for strings
#include <sstream>			// used for streaming text
#include <stdio.h>	
#include "Image_Loading/nvImage.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "OBB.h"
#include "playerCharacter.h" // Header file for the player character 
#include "Platform.h" // Header file for the platforms  
#include "NPC.h" // Header file for the enemy characters
#include "Spaceship.h" // Header file for the spaceship
#include "MovingPlatform.h" // Header file for the moving platforms.
#include <vector>
#include <chrono>
#include "Duration.h"

using namespace std;
using namespace irrklang;

ISoundEngine *SoundEngine = createIrrKlangDevice();

//Variables for the screenSize and the keys
int screenWidth=640, screenHeight=640;
bool keys[256];

int numFrames = 0;

//variables for storing which screens to display
bool displayMenuScreen = true;
bool displayInstructions = false;
bool displayWin = false;
bool displayDefeat = false;
bool displayEnterName = false;
bool instructionsIconLoaded = false;
bool startGameIconLoaded = false;
string typedName = "";

bool playScream = true;

//for storing the loaded in file data
vector<string> names = {};
vector<Duration*> times = {};
int indexToAdd;

//Variables for adjusting the viewport
//Xdir and Ydir is the amount to move the objects by 
float Xdir = 0;
float Ydir = 0;

//Object for handling the player character
PlayerCharacter player;
Spaceship spaceship;


//enemys xspeed, the colour, the max X distance, num lives, damage inflicted, whether the enemy can jump and the speed of the jump
NPC enemy1(10.0f,"blue", 1200.0f,1,1,false,70.0f);
NPC enemy2(15.0f, "green", 1100.0f, 1, 1,true, 70.0f);
//NPC enemy3(25.0f, "red", 700.0f, 1, 1, false, 70.0f);
NPC enemy4(25.0f, "red", 1400.0f, 1, 1, false, 70.0f);
//NPC enemy5(-25.0f, "red", 1400.0f, 1, 1, false, 70.0f);
NPC enemy6(15.0f, "armour", 900.0f, 2, 1, false, 70.0f);
NPC enemy7(25.0f, "boss", 2500.0f, 2, 2, true, 70.0f);
vector<NPC*> enemys = {&enemy1,&enemy2,&enemy4,&enemy6,&enemy7 };


//List of platforms
Platform groundPlat1;
Platform groundPlat2;
Platform groundPlat3;
Platform lev1Plat1;
Platform lev3Plat1;
Platform lev4Plat1;
Platform lev6Plat1;
Platform lev7Plat1;
Platform lev8Plat3;


//List of moving platforms
MovingPlatform lev2Plat1(20.0f, 660.0f, 0.0f, 0.0f);
MovingPlatform lev5Plat1(10.0f, 1000.0f, 0.0f, 0.0f);
MovingPlatform lev5Plat2(-10.0f, 1000.0f, 0.0f, 0.0f);
MovingPlatform lev8Plat1(-10.0f, 1000.0f, 0.0f, 0.0f);
MovingPlatform lev8Plat2(10.0f, 1000.0f, 0.0f, 0.0f);


vector<Platform*> platforms = { &groundPlat1,&groundPlat2,&groundPlat3,&lev1Plat1,&lev2Plat1,&lev3Plat1,&lev4Plat1,&lev5Plat1,&lev5Plat2,&lev6Plat1,&lev7Plat1,&lev8Plat1,&lev8Plat2,&lev8Plat3};
vector<MovingPlatform*> movingPlatforms = { &lev2Plat1,&lev5Plat1 ,&lev5Plat2,&lev8Plat1,&lev8Plat2 };

//Simulation properties
double dt = 0;
__int64 prevTime = 0;
double timeFrequencyRecip = 0.0000065; // Only needs to be changed to change speed of simulation but is platform independent
										// Smaller values will slow down the simulation, larger values will speed it up
										//0.0000075 recommended for PC, 0.000003 for my laptop.
double counter = 0;
//Textures
GLuint background = 0;
GLuint livesIcon = 0;
//Textures for the title screen
GLuint backgroundTitle = 0;
GLuint startGameButton = 0;
GLuint instructionsButton = 0;
GLuint startGameButtonIcon = 0;
GLuint instructionsButtonIcon = 0;
GLuint instructionsImage = 0;
GLuint gameOver = 0;
GLuint home = 0;
GLuint quit = 0;
GLuint youWin = 0;
GLuint enterName = 0;
GLuint pressEnter = 0;

//Duration object to represent the time taken to complete the game
Duration gameTime;
auto start_time = std::chrono::steady_clock::now();
int currentSecondsElapsed = 0;

//Function for loading in an image
GLuint loadPNG(char* name);



GLuint	base;				// Base Display List For The Font Set
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

//Recording mouse input
float mouse_x = 0;
float mouse_y = 0;
bool leftPressed = false;


//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void update();				//called in winmain to update variables
void rescaleWindow();		//called whenever the viewport needs to be changed by resizing the window or 
							//when the game character thretens to leave the screen
void TimeSimulation();

//Display functions
void displayWorld();		//Method for displaying the game's world.
void displayScore();		//Method for displaying the scoring system.
void detectCollisions();	//Method for updating the collisions in the game.
void displayTitle();
void resetWorld();			//Method to reset the world should the user want to play again
void isOnLeaderboard();		//Method for text



/*************    START OF OPENGL FUNCTIONS   ****************/
GLvoid BuildFont(int fontHeight, int fontWidth)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(-fontHeight,							// Height Of Font
		-fontWidth,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_NORMAL,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,		// Family And Pitch
		"Times New Roman");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

GLvoid glPrint(string fmt)					// Custom GL "Print" Routine
{
	stringstream stream;
	stream << fmt;

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
														// first 32 chars not used
	glCallLists(stream.str().size(), GL_UNSIGNED_BYTE, stream.str().c_str());	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}

void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
	glLoadIdentity();

	rescaleWindow();

	if (displayMenuScreen || displayInstructions || displayWin || displayDefeat || displayEnterName) {
		displayTitle();
	}
	else {
		displayWorld();

		//to update the time for the scoreboard
		if (!spaceship.isCollidingPlayer) {
			auto current_time = std::chrono::steady_clock::now();
			currentSecondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
			gameTime.convertFromSeconds(currentSecondsElapsed);
		}
		

		displayScore();

		detectCollisions();
	}

	glFlush();
}

void isOnLeaderboard() {

}

void resetWorld() {
	player.resetCharacter();
	spaceship.resetCharacter();

	for (vector<NPC*>::iterator it = enemys.begin();
		it != enemys.end(); it++) {

		NPC * npc = *it;

		npc->resetCharacter();
	}
	BuildFont((int)round((double)screenHeight / 40.0), (int)round((double)screenHeight / 75.0));

	typedName = "";
	names.clear();
	times.clear();
	playScream = true;
}

void displayWorld() {

	float matrix[16];

	if (player.YPla == 0) {
		Ydir = 0;
	}

	//the background
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, background);
	glPushMatrix();
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
			glTexCoord2f(0 + Ydir / (screenHeight * 8.0), 0 + Xdir/(screenWidth * 8.0)); glVertex2f(0, 0);
			glTexCoord2f(0 + Ydir / (screenHeight * 8.0), 4 + Xdir / (screenWidth * 8.0)); glVertex2f(screenWidth*2.0, 0);
			glTexCoord2f(4 + Ydir / (screenHeight * 8.0), 4 + Xdir / (screenWidth * 8.0)); glVertex2f(screenWidth*2.0, screenHeight*2.0);
			glTexCoord2f(4 + Ydir / (screenHeight * 8.0), 0 + Xdir / (screenWidth * 8.0)); glVertex2f(0, screenHeight*2.0);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	//Character polygon
	glPushMatrix();
		glTranslatef(player.XPla - Xdir, player.YPla - Ydir, 0.0);
		player.addPointsandDraw(50.0, 120.0, 50.0, 240.0, 150.0, 240.0, 150.0, 120.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		player.createOBB(matrix);
	glPopMatrix();

	//Platforms
	glColor3f(1, 1, 1);
	glPushMatrix();
		groundPlat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		groundPlat1.createPlatformAndDraw(-500, 0, -500, 120, 1600, 120, 1600, 0,1200);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		groundPlat1.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		groundPlat2.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		groundPlat2.createPlatformAndDraw(2000, 0, 2000, 120, 2500, 120, 2500, 0, 600);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		groundPlat2.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		groundPlat3.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		groundPlat3.createPlatformAndDraw(2900, 0, 2900, 120, 4300, 120, 4300, 0, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		groundPlat3.createOBB(matrix);
	glPopMatrix();

	/////////////////////////////////////////////////////////////////////
	glPushMatrix();
		glTranslatef(-Xdir + enemy1.XPla, -Ydir + enemy1.YPla, 0.0);
		enemy1.addPointsandDraw(2900, 120, 2900, 270, 3000, 270, 3000, 120);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy1.createOBB(matrix);
	glPopMatrix();
	/////////////////////////////////////////////////////////////////////

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev1Plat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		lev1Plat1.createPlatformAndDraw(4550, 300, 4550, 420, 5050, 420, 5050, 300, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev1Plat1.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev2Plat1.textureWrapType = 1;
		glTranslatef(-Xdir + lev2Plat1.XPla, -Ydir + lev2Plat1.YPla, 0.0);
		lev2Plat1.createPlatformAndDraw(3390, 600, 3390, 720, 3890, 720, 3890, 600, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev2Plat1.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev3Plat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		lev3Plat1.createPlatformAndDraw(1990, 900, 1990, 1020, 3290, 1020, 3290, 900, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev3Plat1.createOBB(matrix);
	glPopMatrix();

	/////////////////////////////////////////////////////////////////////
	glPushMatrix();
		glTranslatef(-Xdir + enemy2.XPla, -Ydir + enemy2.YPla, 0.0);
		enemy2.addPointsandDraw(1990, 1020, 1990, 1170, 2090, 1170,2090, 1020);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy2.createOBB(matrix);
	glPopMatrix();
	/////////////////////////////////////////////////////////////////////

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev4Plat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		lev4Plat1.createPlatformAndDraw(1000, 1200, 1000, 1320, 1790, 1320, 1790, 1200, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev4Plat1.createOBB(matrix);
	glPopMatrix();

	/////////////////////////////////////////////////////////////////////
	/*
	glPushMatrix();
		glTranslatef(-Xdir + enemy3.XPla, -Ydir + enemy3.YPla, 0.0);
		enemy3.addPointsandDraw(1000, 1320, 1000, 1470, 1100, 1470, 1100, 1320);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy3.createOBB(matrix);
	glPopMatrix();
	*/
	/////////////////////////////////////////////////////////////////////

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev5Plat1.textureWrapType = 1;
		glTranslatef(-Xdir + lev5Plat1.XPla, -Ydir + lev5Plat1.YPla, 0.0);
		lev5Plat1.createPlatformAndDraw(1890, 1500, 1890, 1620, 2590, 1620, 2590, 1500, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev5Plat1.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev5Plat2.textureWrapType = 1;
		glTranslatef(-Xdir + lev5Plat2.XPla, -Ydir + lev5Plat2.YPla, 0.0);
		lev5Plat2.createPlatformAndDraw(4790, 1500, 4790, 1620, 5490, 1620, 5490, 1500, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev5Plat2.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev6Plat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		lev6Plat1.createPlatformAndDraw(5690, 1800, 5690, 1920, 7290, 1920, 7290, 1800, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev6Plat1.createOBB(matrix);
	glPopMatrix();

	/////////////////////////////////////////////////////////////////////
	glPushMatrix();
		glTranslatef(-Xdir + enemy4.XPla, -Ydir + enemy4.YPla, 0.0);
		enemy4.addPointsandDraw(5690, 1920, 5690, 2070, 5790, 2070, 5790, 1920);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy4.createOBB(matrix);
	glPopMatrix();

	/*
	glPushMatrix();
		glTranslatef(-Xdir + enemy5.XPla, -Ydir + enemy5.YPla, 0.0);
		enemy5.addPointsandDraw(7140, 1920, 7140, 2070, 7240, 2070, 7240, 1920);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy5.createOBB(matrix);
	glPopMatrix();
	*/
	/////////////////////////////////////////////////////////////////////

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev7Plat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		lev7Plat1.createPlatformAndDraw(7490, 2100, 7490, 2220, 8490, 2220, 8490, 2100, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev7Plat1.createOBB(matrix);
	glPopMatrix();

	/////////////////////////////////////////////////////////////////////
	glPushMatrix();
		glTranslatef(-Xdir + enemy6.XPla, -Ydir + enemy6.YPla, 0.0);
		enemy6.addPointsandDraw(7490, 2220, 7490, 2370, 7590, 2370, 7590, 2220);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy6.createOBB(matrix);
	glPopMatrix();
	/////////////////////////////////////////////////////////////////////

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev8Plat1.textureWrapType = 1;
		glTranslatef(-Xdir + lev8Plat1.XPla, -Ydir + lev8Plat1.YPla, 0.0);
		lev8Plat1.createPlatformAndDraw(6590, 2400, 6590, 2520, 7390, 2520, 7390, 2400, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev8Plat1.createOBB(matrix);
	glPopMatrix();

	
	glColor3f(1, 1, 1);
	glPushMatrix();
		lev8Plat2.textureWrapType = 1;
		glTranslatef(-Xdir + lev8Plat2.XPla, -Ydir + lev8Plat2.YPla, 0.0);
		lev8Plat2.createPlatformAndDraw(3590, 2400, 3590, 2520, 4390, 2520, 4390, 2400, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev8Plat2.createOBB(matrix);
	glPopMatrix();

	glColor3f(1, 1, 1);
	glPushMatrix();
		lev8Plat3.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		lev8Plat3.createPlatformAndDraw(90, 2400, 90, 2520, 3290, 2520, 3290, 2400, 900);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		lev8Plat3.createOBB(matrix);
	glPopMatrix();
	
	/////////////////////////////////////////////////////////////////////
	glPushMatrix();
	glTranslatef(-Xdir + enemy7.XPla, -Ydir + enemy7.YPla, 0.0);
		enemy7.addPointsandDraw(450, 2520, 450, 2720, 600, 2720, 600, 2520);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy7.createOBB(matrix);
	glPopMatrix();
	/////////////////////////////////////////////////////////////////////

	//the spaceship that marks the end of the game
	glPushMatrix();
		glTranslatef(-Xdir + spaceship.XPla, -Ydir + spaceship.YPla, 0.0);
		spaceship.addPointsandDraw(100, 2520, 100, 2900, 400, 2900, 400, 2520);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		spaceship.createOBB(matrix);
	glPopMatrix();

	if (playScream) {
		if (player.YPla < -120) {
			SoundEngine->play2D("audio/scream.mp3", GL_FALSE);
			playScream = false;
		}
	}
	
	if (player.YPla == 0) {
		playScream = true;
	}
}

void displayScore() {

	// Position The Text On The Screen
	//print the elapsed time
	string secString;
	string minString;
	string hourString;

	if (gameTime.seconds < 10) {
		secString = "0" + std::to_string(gameTime.seconds);
	}
	else {
		secString = std::to_string(gameTime.seconds);
	}

	if (gameTime.minutes < 10) {
		minString = "0" + std::to_string(gameTime.minutes);
	}
	else {
		minString = std::to_string(gameTime.minutes);
	}

	if (gameTime.hours < 10) {
		hourString = "0" + std::to_string(gameTime.hours);
	}
	else {
		hourString = std::to_string(gameTime.hours);
	}

	glRasterPos2f(screenWidth * 2*0.6, screenHeight * 2 - 100);
	glColor3f(1.0, 1.0, 1.0);
	glPrint("Time Taken: " + hourString + ":" + minString + ":" + secString);	// Print GL Text To The Screen

	//Now display the health of the player
	//the background

	int Xdirection = 120;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, livesIcon);
	for (int i = 0; i < player.livesLeft; i++) {
		glPushMatrix();
			glTranslatef(Xdirection, screenHeight*2.0 - 150.0, 0.0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(0, 1); glVertex2f(0, 100);
				glTexCoord2f(1, 1); glVertex2f(70, 100);
				glTexCoord2f(1, 0); glVertex2f(70, 0);
			glEnd();
		glPopMatrix();
		Xdirection += 60;
	}
	glDisable(GL_TEXTURE_2D);
}

void detectCollisions() {

	if(!spaceship.isCollidingPlayer){
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//player collisions
		//Platform collisions

		//This part is for testing which platforms are colliding with the player
		bool isColliding = false;
		vector<Platform*> platformsToTest = {};
		vector<NPC*> enemiesToTest = {};
		int increment = 50;

		if (player.YPla > 0-increment && player.YPla < 180+increment) {
			platformsToTest = { &groundPlat1,&groundPlat2,&groundPlat3,&lev1Plat1};
			enemiesToTest = {&enemy1};
		} else if (player.YPla > 300-increment && player.YPla < 480+increment) {
			platformsToTest = { &groundPlat1,&groundPlat2,&groundPlat3,&lev1Plat1,&lev2Plat1};
			enemiesToTest = {};
		}
		else if (player.YPla > 600 - increment && player.YPla < 780 + increment) {
			platformsToTest = { &lev1Plat1,&lev2Plat1,&lev3Plat1 };
			enemiesToTest = {};
		}
		else if (player.YPla > 900 - increment && player.YPla < 1080 + increment) {
			platformsToTest = { &lev2Plat1,&lev3Plat1,&lev4Plat1 };
			enemiesToTest = { &enemy2 };
		}
		else if (player.YPla > 1200 - increment && player.YPla < 1380 + increment) {
			platformsToTest = { &lev3Plat1,&lev4Plat1,&lev5Plat1,&lev5Plat2 };
			enemiesToTest = { &enemy2 };
		}
		else if (player.YPla > 1500 - increment && player.YPla < 1680 + increment) {
			platformsToTest = { &lev4Plat1,&lev5Plat1,&lev5Plat2,&lev6Plat1 };
			enemiesToTest = {};
		}
		else if (player.YPla > 1800 - increment && player.YPla < 1980 + increment) {
			platformsToTest = { &lev5Plat1,&lev5Plat2,&lev6Plat1,&lev7Plat1 };
			enemiesToTest = {&enemy4};
		}
		else if (player.YPla > 2100 - increment && player.YPla < 2280 + increment) {
			platformsToTest = { &lev6Plat1,&lev7Plat1,&lev8Plat1,&lev8Plat2,&lev8Plat3 };
			enemiesToTest = {&enemy6};
		}
		else if (player.YPla > 2400 - increment && player.YPla < 2580 + increment) {
			platformsToTest = { &lev7Plat1,&lev8Plat1,&lev8Plat2,&lev8Plat3 };
			enemiesToTest = {&enemy7};
		}
		else  {
			platformsToTest = { &lev8Plat1,&lev8Plat2,&lev8Plat3 };
			enemiesToTest = { &enemy7 };
		}

		for (vector<Platform*>::iterator it = platformsToTest.begin();
			it != platformsToTest.end(); it++) {

			Platform * platform = *it;

			//For handling collisions
			isColliding = player.boundingBox.SAT2D(platform->boundingBox);

			//if it's colliding with the ground then set the collision status to that object corresponding to that object
			if (isColliding) {
				player.areCollidingPlatform = true;
				player.collisionStatuses.push_back(platform->typeOfCollision(player, dt));
			}
		}
		platformsToTest.clear();

		//so the player is not colliding with anything at all
		if (player.collisionStatuses.empty()) {
			player.areCollidingPlatform = false;
			//so the player can't jump while in mid air
			player.jumpPressed = true;
		}

		//NPC Collisions
		//This part is for testing which NPCs are colliding with the player
		isColliding = false;

		for (vector<NPC*>::iterator it = enemiesToTest.begin();
			it != enemiesToTest.end(); it++) {

			NPC * npc = *it;

			//For handling collisions
			isColliding = player.boundingBox.SAT2D(npc->boundingBox);

			if (isColliding) {
				npc->typeOfCollision(player, dt, SoundEngine);
			}
		}

		//test if the player has reached the end of the game
		spaceship.isCollidingPlayer = player.boundingBox.SAT2D(spaceship.boundingBox);
		player.collidingSpaceship = player.boundingBox.SAT2D(spaceship.boundingBox);

		if (player.collidingSpaceship) {
			player.Yspeed = 2.0f;
			spaceship.Yspeed = 2.0f;
			spaceship.loadTexture("Sprites/spaceshipLiftOff.png");


			SoundEngine->play2D("audio/woohoo.flac", GL_FALSE);
			SoundEngine->play2D("audio/liftoff.wav", GL_FALSE);
			
		}
	}
	else {
		player.textureID = 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//enemy collisions

	//This part is for testing which platforms are colliding with the enemies
	for (int i = 0; i < enemys.size(); i++) {
		bool isColliding = false;
		Platform * platform = NULL;

		//i corresponds to the NPC number
		if (i == 0) {
			platform = platforms[2];
		}
		else if (i == 1) {
			platform = platforms[5];
		}
		//else if (i == 2) {
			//platform = platforms[6];
		//}
		else if (i == 2) {
			platform = platforms[9];
		}
		//else if (i == 3) {
			//platform = platforms[8];
		//}
		else if (i == 3) {
			platform = platforms[10];
		}
		else if (i == 4) {
			platform = platforms[13];
		}

		//For handling collisions
		isColliding = enemys[i]->boundingBox.SAT2D(platform->boundingBox);

		//if it's colliding with the ground then set the collision status to that object corresponding to that object
		if (isColliding) {
			enemys[i]->areCollidingPlatform = true;
			enemys[i]->collisionStatuses.push_back(platform->typeOfCollision(*enemys[i], dt));
		}
			

		//so the player is not colliding with anything at all
		if (enemys[i]->collisionStatuses.empty()) {
			enemys[i]->areCollidingPlatform = false;
		}
	}
	
	
	if (player.livesLeft <= 0) {
		displayDefeat = true;
		startGameButtonIcon = loadPNG("Sprites/astronautStill.png");
		instructionsButtonIcon = loadPNG("Sprites/background.png");
	}

	//Test if the player has made it onto the leaderboard
	if (spaceship.Yspeed > 100) {

		ifstream inFile("leaderboard.csv");

		string line;

		//gather the time from the leaderboard file to see if the person has made it
		while (inFile >> line) {
			stringstream check(line);

			string intermediate;

			vector<string> tokens;

			while (getline(check, intermediate, ',')) {
				tokens.push_back(intermediate);	
			}

			for (int i = 0; i < tokens.size(); i++) {
				//its a name
				if (i % 2 == 0) {
					names.push_back(tokens[i]);
				}
				else {
					//otherwise its a time
					string currentTime = tokens[i];

					Duration * timeToAdd = new Duration();

					stringstream check2(currentTime);

					string intermediate2;

					vector<string> timeTokens;

					while (getline(check2, intermediate2, ':')) {
						timeTokens.push_back(intermediate2);
					}

					int newHours = stoi(timeTokens[0]);

					int newMinutes = stoi(timeTokens[1]);

					int newSeconds = stoi(timeTokens[2]);

					timeToAdd->hours = newHours;

					timeToAdd->minutes = newMinutes;

					timeToAdd->seconds = newSeconds;

					times.push_back(timeToAdd);
				}
			}
				
		}

		//then test if the player has made it onto the leaderboard

		for (int i = 0; i < times.size(); i++) {

			Duration timeToTest = *times[i];

			if (gameTime.hours < timeToTest.hours) {
				displayEnterName = true;
				numFrames = 0;
				indexToAdd = i;
				break;
			}
			else {
				if (gameTime.hours == timeToTest.hours) {
					if (gameTime.minutes < timeToTest.minutes) {
						displayEnterName = true;
						indexToAdd = i;
						break;
					}
					else {
						if (gameTime.minutes == timeToTest.minutes) {
							if (gameTime.seconds < timeToTest.seconds) {
								displayEnterName = true;
								indexToAdd = i;
								break;
							}
						}
					}
				}
			}
		}
		if (!displayEnterName) {
			displayWin = true;
			indexToAdd = -1;
		}
		
	}
}

//Method for controlling the menu that appears throughout the game such as the start menu, instructions, game over screen
//and completed game screen.
void displayTitle() {

	if (displayMenuScreen) {
		numFrames = 0;
		if (mouse_y > screenHeight / 2.0 && mouse_y < screenHeight*0.7) {
			instructionsIconLoaded = false;

			if (!startGameIconLoaded) {
				startGameButtonIcon = loadPNG("Sprites/astronautStill.png");
				instructionsButtonIcon = loadPNG("Sprites/background.png");
				startGameIconLoaded = true;
			}
			if (leftPressed) {
				//if these are all false then load the game
				displayMenuScreen = false;
				displayInstructions = false;
				displayWin = false;
				displayDefeat = false;
				displayEnterName = false;
				start_time = std::chrono::steady_clock::now();
			}
		}
		else if (mouse_y < screenHeight / 2.0 && mouse_y > screenHeight*0.3) {
			startGameIconLoaded = false;

			if (!instructionsIconLoaded) {
				instructionsButtonIcon = loadPNG("Sprites/astronautStill.png");
				startGameButtonIcon = loadPNG("Sprites/background.png");
				instructionsIconLoaded = true;
			}
			if (leftPressed) {
				//just instructions true so load the instructions page
				displayMenuScreen = false;
				displayInstructions = true;
				counter = 0;
				displayWin = false;
				displayDefeat = false;
				displayEnterName = false;
			}
		}
		else {

			if (startGameIconLoaded) {
				startGameButtonIcon = loadPNG("Sprites/background.png");
				startGameIconLoaded = false;
			}
			
			if (instructionsIconLoaded) {
				instructionsButtonIcon = loadPNG("Sprites/background.png");
				instructionsIconLoaded = false;
			}
		}
	}
	else if (displayEnterName) {

	}
	else if (displayWin) {

		//for displaying the buttons and titles
		if (mouse_x > 0 && mouse_x < 300)
		{
			startGameIconLoaded = false;
			if (!instructionsIconLoaded) {
				startGameButtonIcon = loadPNG("Sprites/astronautStill.png");
				instructionsButtonIcon = loadPNG("Sprites/background.png");
				instructionsIconLoaded = true;
			}
			if (leftPressed) {
				resetWorld();
				displayMenuScreen = true;
				displayInstructions = false;
				displayWin = false;
				displayDefeat = false;
				displayEnterName = false;
			}
		}
		else if (mouse_x > 320 && mouse_x < screenWidth*2.0) {

			instructionsIconLoaded = false;

			if (!startGameIconLoaded) {
				instructionsButtonIcon = loadPNG("Sprites/astronautStill.png");
				startGameButtonIcon = loadPNG("Sprites/background.png");
				startGameIconLoaded = true;
			}
			if (leftPressed) {
				exit(EXIT_SUCCESS);
			}
		}
		else {

			if (startGameIconLoaded) {
				startGameButtonIcon = loadPNG("Sprites/background.png");
				startGameIconLoaded = false;
			}

			if (instructionsIconLoaded) {
				instructionsButtonIcon = loadPNG("Sprites/background.png");
				instructionsIconLoaded = false;
			}

		}
	}
	else if (displayDefeat) {
		numFrames = 0;
		if (mouse_x > 0 && mouse_x < 300) 
		{
			startGameIconLoaded = false;
			if (!instructionsIconLoaded) {
				startGameButtonIcon = loadPNG("Sprites/astronautStill.png");
				instructionsButtonIcon = loadPNG("Sprites/background.png");
				instructionsIconLoaded = true;
			}
			if (leftPressed) {
				resetWorld();
				displayMenuScreen = true;
				displayInstructions = false;
				displayWin = false;
				displayDefeat = false;
				displayEnterName = false;
			}
		}
		else if (mouse_x > 320 && mouse_x < screenWidth*2.0) {

			instructionsIconLoaded = false;

			if (!startGameIconLoaded) {
				instructionsButtonIcon = loadPNG("Sprites/astronautStill.png");
				startGameButtonIcon = loadPNG("Sprites/background.png");
				startGameIconLoaded = true;
			}
			if (leftPressed) {
				exit(EXIT_SUCCESS);
			}
		}
		else {

			if (startGameIconLoaded) {
				startGameButtonIcon = loadPNG("Sprites/background.png");
				startGameIconLoaded = false;
			}

			if (instructionsIconLoaded) {
				instructionsButtonIcon = loadPNG("Sprites/background.png");
				instructionsIconLoaded = false;
			}

		}
	}
	else if (displayInstructions) {
		counter += dt;
		if (counter > 20.0) {
			if (leftPressed) {
				displayMenuScreen = false;
				displayInstructions = false;
				displayWin = false;
				displayDefeat = false;
				displayEnterName = false;
				start_time = std::chrono::steady_clock::now();
				numFrames = 0;
			}
		}
	}
	

	//the background
	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, background);
		glPushMatrix();
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(0, 0);
			glTexCoord2f(0, 4); glVertex2f(screenWidth*2.0, 0);
			glTexCoord2f(4, 4); glVertex2f(screenWidth*2.0, screenHeight*2.0);
			glTexCoord2f(4, 0); glVertex2f(0, screenHeight*2.0);
		glEnd();
		glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//drawing the design for each screen
	if (displayMenuScreen) {
		//start button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, startGameButton);
			glPushMatrix();
			glTranslatef(330, screenHeight * 2 - 600, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(screenWidth, 0);
				glTexCoord2f(1, 1); glVertex2f(screenWidth, (screenHeight*2.0) / 8.0);
				glTexCoord2f(0, 1); glVertex2f(0, (screenHeight*2.0) / 8.0);
			glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, startGameButtonIcon);
			glPushMatrix();
			glTranslatef(240, 700, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(80, 0);
				glTexCoord2f(1, 1); glVertex2f(80, 120);
				glTexCoord2f(0, 1); glVertex2f(0, 120);
			glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//instructions button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, instructionsButton);
			glPushMatrix();
			glTranslatef(330, screenHeight * 2 - 800, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(screenWidth, 0);
				glTexCoord2f(1, 1); glVertex2f(screenWidth, (screenHeight*2.0) / 8.0);
				glTexCoord2f(0, 1); glVertex2f(0, (screenHeight*2.0) / 8.0);
			glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, instructionsButtonIcon);
			glPushMatrix();
			glTranslatef(240, 500, 0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(80, 0);
				glTexCoord2f(1, 1); glVertex2f(80, 120);
				glTexCoord2f(0, 1); glVertex2f(0, 120);
			glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
	else if (displayInstructions) {
		//the background
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, instructionsImage);
			glPushMatrix();
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(screenWidth*2.0, 0);
				glTexCoord2f(1, 1); glVertex2f(screenWidth*2.0, screenHeight*2.0);
				glTexCoord2f(0, 1); glVertex2f(0, screenHeight*2.0);
			glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
	else if (displayDefeat) {
		//display the game over screen
		//game over title
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, gameOver);
			glPushMatrix();
				glTranslatef(330, screenHeight * 2-250, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth, (screenHeight*2.0) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight*2.0) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//home button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, home);
			glPushMatrix();
				glTranslatef(150, 100, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth*0.5, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth*0.5, (screenHeight) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, startGameButtonIcon);
			glPushMatrix();
				glTranslatef(50, 75, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(80, 0);
					glTexCoord2f(1, 1); glVertex2f(80, 120);
					glTexCoord2f(0, 1); glVertex2f(0, 120);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//quit button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, quit);
			glPushMatrix();
				glTranslatef(800, 100, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth*0.5, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth*0.5, (screenHeight) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, instructionsButtonIcon);
			glPushMatrix();
				glTranslatef(700, 75, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(80, 0);
					glTexCoord2f(1, 1); glVertex2f(80, 120);
					glTexCoord2f(0, 1); glVertex2f(0, 120);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
	else if (displayWin) {
		//display the you win screen
		//you win title
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, youWin);
			glPushMatrix();
				glTranslatef(330, screenHeight * 2 - 250, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth, (screenHeight*2.0) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight*2.0) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//home button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, home);
			glPushMatrix();
				glTranslatef(150, 100, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth*0.5, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth*0.5, (screenHeight) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, startGameButtonIcon);
			glPushMatrix();
				glTranslatef(50, 75, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(80, 0);
					glTexCoord2f(1, 1); glVertex2f(80, 120);
					glTexCoord2f(0, 1); glVertex2f(0, 120);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//quit button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, quit);
			glPushMatrix();
				glTranslatef(800, 100, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth*0.5, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth*0.5, (screenHeight) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, instructionsButtonIcon);
			glPushMatrix();
				glTranslatef(700, 75, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(80, 0);
					glTexCoord2f(1, 1); glVertex2f(80, 120);
					glTexCoord2f(0, 1); glVertex2f(0, 120);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//display the leaderboard
		if (numFrames == 0) {
			BuildFont(30, 15);
		}

		for (int i = 0; i < times.size(); i++) {

			if (i == indexToAdd) {
				glColor3f(1.0, 0.0, 0.0);
			}
			else {
				glColor3f(1.0, 1.0, 1.0);
			}
			glRasterPos2f(400, 800 - i * 50);


			string hoursString;
			string minutesString;
			string secondsString;
			stringstream ss;

			ss << times[i]->hours;

			hoursString = ss.str();

			stringstream ss1;
			ss1 << times[i]->minutes;

			minutesString = ss1.str();

			stringstream ss2;
			ss2 << times[i]->seconds;

			secondsString = ss2.str();

			if (times[i]->hours < 10) {
				hoursString.insert(0, "0");
			}

			if (times[i]->minutes < 10) {
				minutesString.insert(0, "0");
			}

			if (times[i]->seconds < 10) {
				secondsString.insert(0, "0");
			}

			//print the names
			glPrint(names[i]);

			//and their times
			glRasterPos2f(800, 800 - i * 50);
			glPrint(hoursString + ":" + minutesString + ":" + secondsString);


			stringstream ss3;
			ss3 << (i + 1);


			glRasterPos2f(200, 800 - i * 50);
			glPrint(ss3.str());
		}
			
		numFrames++;
		
	}
	else if (displayEnterName) {
		//display the win screen
		//display the game over screen
		//you win title
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, enterName);
			glPushMatrix();
				glTranslatef(330, screenHeight * 2 - 250, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(screenWidth, 0);
					glTexCoord2f(1, 1); glVertex2f(screenWidth, (screenHeight*2.0) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight*2.0) / 8.0);
				glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//continue button
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pressEnter);
			glPushMatrix();
				glTranslatef(100, 100, 0);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(1, 0); glVertex2f(1080, 0);
					glTexCoord2f(1, 1); glVertex2f(1080, (screenHeight) / 8.0);
					glTexCoord2f(0, 1); glVertex2f(0, (screenHeight) / 8.0);
			glEnd();
			glPopMatrix();
		glDisable(GL_TEXTURE_2D);

		//display the typed name
		if (numFrames == 0) {
			BuildFont(100, 50);
		}
		
		numFrames++;
		glRasterPos2f(250, screenHeight);
		glColor3f(1.0, 1.0, 1.0);
		glPrint(typedName);
		
	}
}



void reshape(int width, int height)		// Resize the OpenGL window
{
	if (height != 0 || width != 0) {
		Xdir = (screenWidth / width)*player.XPla - Xdir;	//Change the X and Y Position of the viewport depending on 
		Ydir = (screenHeight / height)*Ydir;				//the change of the viewport
	}
	

	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset the current viewport

	glMatrixMode(GL_PROJECTION);						// select the projection matrix stack
	glLoadIdentity();									// reset the top of the projection matrix to an identity matrix

	gluOrtho2D(0,screenWidth*2,0,screenHeight*2);           // set the coordinate system for the window

	glMatrixMode(GL_MODELVIEW);							// Select the modelview matrix stack
	glLoadIdentity();									// Reset the top of the modelview matrix to an identity matrix

	BuildFont((int)round((double)screenHeight / 40.0), (int)round((double)screenHeight / 75.0));
	
}

void rescaleWindow() {

	//For adjusting the viewport when the character is hitting the edge of it
	//For the Xdirection
	//the left edge
	if ((player.XPla-Xdir) < (screenWidth*2.0*0.4f)) {
		//Xdir is the X direction of the viewport to move, 
		Xdir = player.XPla - (screenWidth*2.0*0.4f) + 0.5f*(player.Xspeed+player.oldXspeed)*dt;
	}
	//the right edge
	else if ((player.XPla-Xdir) > (screenWidth*2.0*0.42f)) {
		Xdir = (player.XPla -( screenWidth * 2.0*0.42f) + 0.5f*(player.Xspeed + player.oldXspeed)*dt);
	}
	

	//For the Ydirection
	//the bottom edge
	if (player.YPla < (screenHeight*2.0*0.154f - 0.3f*screenHeight*2.0)) {
		Ydir = player.YPla - (screenHeight*2.0*0.05f - 0.3f*screenHeight*2.0) + 0.5f*(player.Yspeed + player.oldYspeed)*dt;
	}
	//the top edge
	else if (player.YPla > (screenHeight*2.0*0.4f)) {
		Ydir = (player.YPla - screenHeight * 2.0*0.4f + 0.5f*(player.Yspeed + player.oldYspeed)*dt);
	}
}

GLuint loadPNG(char* name)
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);

	return myTextureID;
}

void init()
{
	glClearColor(0.0,0.0,0.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour.


	BuildFont((int)round((double)screenHeight / 35.0), (int)round((double)screenHeight / 75.0));

	//Only call loadPNG on the background
	background = loadPNG("Sprites/background.png");
	livesIcon = loadPNG("Sprites/astronautStill.png");
	startGameButton = loadPNG("Sprites/startGame.png");
	instructionsButton = loadPNG("Sprites/instructions.png");
	backgroundTitle = loadPNG("Sprites/Ground/ground5.png");
	instructionsImage = loadPNG("Sprites/instructionsPage.png");
	gameOver = loadPNG("Sprites/gameOver.png");
	youWin = loadPNG("Sprites/youWin.png");
	home = loadPNG("Sprites/home.png");
	quit = loadPNG("Sprites/quit.png");
	enterName = loadPNG("Sprites/enterName.png");
	pressEnter = loadPNG("Sprites/pressEnter.png");
	startGameButtonIcon = loadPNG("Sprites/astronautStill.png");
	instructionsButtonIcon = loadPNG("Sprites/background.png");

	
	player.loadTexture("Sprites/astronautStill.png");
	
	spaceship.loadTexture("Sprites/spaceshipStill.png");

	groundPlat1.loadTexture("Sprites/platform22by2.png");
	groundPlat2.loadTexture("Sprites/platform5by2.png");
	groundPlat3.loadTexture("Sprites/platform12by2.png");
	lev1Plat1.loadTexture("Sprites/platform5by2.png");
	lev2Plat1.loadTexture("Sprites/platform7by2.png");
	lev3Plat1.loadTexture("Sprites/platform17by2.png");
	lev4Plat1.loadTexture("Sprites/platform12by2.png");
	lev5Plat1.loadTexture("Sprites/platform12by2.png");
	lev5Plat2.loadTexture("Sprites/platform12by2.png");
	lev6Plat1.loadTexture("Sprites/platform17by2.png");
	lev7Plat1.loadTexture("Sprites/platform12by2.png");
	lev8Plat1.loadTexture("Sprites/platform12by2.png");
	lev8Plat2.loadTexture("Sprites/platform12by2.png");
	lev8Plat3.loadTexture("Sprites/platform29by2.png");

	enemy1.loadTexture("Sprites/alien/blue/blue_walk1.png");
	enemy2.loadTexture("Sprites/alien/blue/blue_walk1.png");
	//enemy3.loadTexture("Sprites/alien/blue/blue_walk1.png");
	enemy4.loadTexture("Sprites/alien/blue/blue_walk1.png");
	//enemy5.loadTexture("Sprites/alien/blue/blue_walk1.png");
	enemy6.loadTexture("Sprites/alien/blue/blue_walk1.png");
	enemy7.loadTexture("Sprites/alien/blue/blue_walk1.png");
}

void processKeys()
{
	if ((!displayMenuScreen || !displayInstructions || !displayWin || !displayDefeat || !displayEnterName) && !spaceship.isCollidingPlayer) {
		if (keys[VK_LEFT])
		{
			player.leftPressed = true;
		}
		else {
			player.leftPressed = false;
		}
		if (keys[VK_RIGHT])
		{
			player.rightPressed = true;
		}
		else {
			player.rightPressed = false;
		}
		if (keys[VK_UP] || keys[VK_SPACE]) {

			//if it's been pressed initially
			if (!player.jumpPressed) {
				player.oldYspeed = 0.0f;
				player.Yspeed = 70.0f;
				player.jumpPressed = true;
				SoundEngine->play2D("audio/jump.wav", GL_FALSE);
			}

			player.jumpCounter++;
		}
		else {
			if (player.jumpPressed) {
				player.jumpCounter--;
			}
			else {
				player.jumpCounter = 0;
			}
		}
	}

	if (displayEnterName && (typedName.size() < 7)) {
		counter += dt;
		if (counter > 8.0) {
			//letter A on the keyboard
			if (keys[0x41]) {
				typedName.append("a");
				counter = 0;
			}

			if (keys[0x42]) {
				typedName.append("b");
				counter = 0;
			}

			if (keys[0x43]) {
				typedName.append("c");
				counter = 0;
			}

			if (keys[0x44]) {
				typedName.append("d");
				counter = 0;
			}

			if (keys[0x45]) {
				typedName.append("e");
				counter = 0;
			}

			if (keys[0x46]) {
				typedName.append("f");
				counter = 0;
			}

			if (keys[0x47]) {
				typedName.append("g");
				counter = 0;
			}

			if (keys[0x48]) {
				typedName.append("h");
				counter = 0;
			}

			if (keys[0x49]) {
				typedName.append("i");
				counter = 0;
			}

			if (keys[0x4A]) {
				typedName.append("j");
				counter = 0;
			}

			if (keys[0x4B]) {
				typedName.append("k");
				counter = 0;
			}

			if (keys[0x4C]) {
				typedName.append("l");
				counter = 0;
			}

			if (keys[0x4D]) {
				typedName.append("m");
				counter = 0;
			}

			if (keys[0x4E]) {
				typedName.append("n");
				counter = 0;
			}

			if (keys[0x4F]) {
				typedName.append("o");
				counter = 0;
			}

			if (keys[0x50]) {
				typedName.append("p");
				counter = 0;
			}

			if (keys[0x51]) {
				typedName.append("q");
				counter = 0;
			}

			if (keys[0x52]) {
				typedName.append("r");
				counter = 0;
			}

			if (keys[0x53]) {
				typedName.append("s");
				counter = 0;
			}

			if (keys[0x54]) {
				typedName.append("t");
				counter = 0;
			}

			if (keys[0x55]) {
				typedName.append("u");
				counter = 0;
			}

			if (keys[0x56]) {
				typedName.append("v");
				counter = 0;
			}

			if (keys[0x57]) {
				typedName.append("w");
				counter = 0;
			}

			if (keys[0x58]) {
				typedName.append("x");
				counter = 0;
			}

			if (keys[0x59]) {
				typedName.append("y");
				counter = 0;
			}

			if (keys[0x5A]) {
				typedName.append("z");
				counter = 0;
			}
		}
	}
	
	if (displayEnterName) {
		if (keys[VK_RETURN]) {
			displayWin = true;
			displayEnterName = false;
			startGameButtonIcon = loadPNG("Sprites/astronautStill.png");
			instructionsButtonIcon = loadPNG("Sprites/background.png");

			//to shift all the elements in the time and name array down as the person has made it onto the leaderboard
			if (typedName.size() == 0) {
				typedName = "player";
			}
			string nameToAdd = typedName;
			Duration * timeToAdd = &gameTime;

			vector<string>newNames = {};
			vector<Duration*> newTimes = {};

			for (int i = 0; i < names.size(); i++) {
				if (i == indexToAdd) {
					newNames.push_back(nameToAdd);
				}
				else if (i > indexToAdd) {
					newNames.push_back(names[(i-1)]);
				}
				else {
					newNames.push_back(names[i]);
				}
			}

			for (int i = 0; i < times.size(); i++) {
				if (i == indexToAdd) {
					newTimes.push_back(timeToAdd);
				}
				else if (i > indexToAdd) {
					newTimes.push_back(times[(i - 1)]);
				}
				else {
					newTimes.push_back(times[i]);
				}
			}

			//copy over the new data into the names and times arrays
			times.clear();
			names.clear();

			for (int i = 0; i < newTimes.size(); i++) {
				times.push_back(newTimes[i]);
			}

			for (int i = 0; i < newNames.size(); i++) {
				names.push_back(newNames[i]);
			}

			numFrames = 0;

			//Save the scoreboard and clear the arrays

			ofstream file("leaderboard.csv");

			if (file.is_open()) {
				for (int i = 0; i < names.size(); i++) {
					string hoursString;
					string minutesString;
					string secondsString;
					stringstream ss;

					ss << times[i]->hours;

					hoursString = ss.str();

					stringstream ss1;
					ss1 << times[i]->minutes;

					minutesString = ss1.str();

					stringstream ss2;
					ss2 << times[i]->seconds;

					secondsString = ss2.str();

					if (times[i]->hours < 10) {
						hoursString.insert(0, "0");
					}

					if (times[i]->minutes < 10) {
						minutesString.insert(0, "0");
					}

					if (times[i]->seconds < 10) {
						secondsString.insert(0, "0");
					}

					file << names[i];
					file << ",";
					file << hoursString;
					file << ":";
					file << minutesString;
					file << ":";
					file << secondsString;
					if (i != (names.size() - 1)) {
						file << "\n";
					}

				}
				file.close();
			}
			else cout << "Unable to open file";
		}
	}
}

void TimeSimulation() {
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	__int64 currentTime = t.QuadPart;

	__int64 ticksElapsed = currentTime - prevTime;

	if (prevTime == 0) {
		dt = 0;
	}
	else {
		dt = double(ticksElapsed)*timeFrequencyRecip;
	}
	

	prevTime = currentTime;
}

void update()
{

	if (!displayMenuScreen && !displayInstructions && !displayWin && !displayDefeat && !displayEnterName) {

		if (dt > 1.0) {
			dt = 0.0f;
		}
		player.updatePlayerMovement(dt);

		for (int i = 0; i < movingPlatforms.size(); i++) {
			movingPlatforms[i]->updatePlatformMovement(dt);
		}

		for (int i = 0; i < enemys.size(); i++) {

			enemys[i]->updatePlayerMovement(dt);

			//update the jumping movement
			if (enemys[i]->jumping) {
				if (!enemys[i]->jumpPressed) {
					enemys[i]->oldYspeed = 0.0f;
					enemys[i]->Yspeed = enemys[i]->jumpSpeed;
					enemys[i]->jumpPressed = true;
				}
			}
		}

		spaceship.updatePlayerMovement(dt);
	}
}
/**************** END OPENGL FUNCTIONS *************************/

//WIN32 functions
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(	HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function


/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	bool	done=false;								// Bool Variable To Exit Loop

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//For displaying a command line console
	//AllocConsole();
	//FILE *stream;
	//freopen_s(&stream, "CONOUT$", "w", stdout);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Create Our OpenGL Window
	if (!CreateGLWindow("Platform Game",screenWidth,screenHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if(keys[VK_ESCAPE])
				done = true;

			processKeys();			//process keyboard		
			display();					// Draw The Scene
			TimeSimulation();
			update();					// update variables
			SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
		}
		
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		break;

		case WM_SIZE:								// Resize The OpenGL Window
		{
			reshape(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
		break;

		case WM_LBUTTONDOWN:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = screenHeight - HIWORD(lParam);
				leftPressed = true;
			}
		break;

		case WM_LBUTTONUP:
			{
	            leftPressed = false;
			}
		break;

		case WM_MOUSEMOVE:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = screenHeight  - HIWORD(lParam);
			}
		break;
		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = true;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}
		break;
		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = false;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}
		break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*/
 
bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}
	
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	reshape(width, height);					// Set Up Our Perspective GL Screen

	init();
	
	return true;									// Success
}



