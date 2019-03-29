
//includes areas for keyboard control, mouse control, resizing the window
//and draws a spinning rectangle

#include <windows.h>		// Header file for Windows
#include <iostream>
#include <string>			// used for strings
#include <sstream>			// used for streaming text
#include "Image_Loading/nvImage.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "OBB.h"
#include "playerCharacter.h" // Header file for the player character 
#include "Platform.h" // Header file for the platforms  
#include "NPC.h" // Header file for the enemy characters
#include "MovingPlatform.h" // Header file for the moving platforms.
#include <vector>
#include <chrono>
#include "Duration.h"

using namespace std;

//Variables for the screenSize and the keys
int screenWidth=640, screenHeight=640;
bool keys[256];

//Variables for adjusting the viewport
//Xdir and Ydir is the amount to move the objects by 
float Xdir = 0;
float Ydir = 0;

//Object for handling the player character
PlayerCharacter player;
NPC enemy;
vector<NPC*> enemys = { &enemy };

//Object for handling the ground platform
Platform ground;
Platform plat1;
Platform plat2;
Platform plat3;
MovingPlatform plat4;
vector<Platform*> platforms = { &ground,&plat1,&plat2,&plat3,&plat4};

//Simulation properties
double dt = 0;
__int64 prevTime = 0;
double timeFrequencyRecip = 0.000003; // Only needs to be changed to change speed of simulation but is platform independent
										// Smaller values will slow down the simulation, larger values will speed it up
										//0.000008 recommended for PC, 0.000003 for my laptop.


//Textures
GLuint background = 0;
GLuint livesIcon = 0;

//Duration object to represent the time taken to complete the game
Duration gameTime;
auto start_time = std::chrono::steady_clock::now();
int currentSecondsElapsed = 0;

//Function for loading in an image
GLuint loadPNG(char* name);

//Boolean to record when the menu of the game is being shown or not
bool menuShowing = true;
GLuint	base;				// Base Display List For The Font Set
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

//Optional added stuff from template
float mouse_x = 0;
float mouse_y = 0;
bool LeftPressed = false;


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

	displayWorld();

	//to update the time for the scoreboard
	auto current_time = std::chrono::steady_clock::now();
	currentSecondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
	gameTime.convertFromSeconds(currentSecondsElapsed);

	displayScore();

	detectCollisions();

	glFlush();
}

void displayWorld() {

	float matrix[16];

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
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glTranslatef(player.XPla - Xdir, player.YPla - Ydir, 0.0);
		player.addPointsandDraw(50.0, 120.0, 50.0, 240.0, 150.0, 240.0, 150.0, 120.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		player.createOBB(matrix);
	glPopMatrix();

	//The surface
	glColor3f(1, 1, 1);
	glPushMatrix();
		glTranslatef(-Xdir, -Ydir, 0.0);
		ground.createPlatformAndDraw(-10000, 0, -10000, 120, 10000, 120, 10000, 0, screenWidth, screenHeight);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		ground.createOBB(matrix);
	glPopMatrix();


	//Platforms
	
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		plat1.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		plat1.createPlatformAndDraw(400, 260, 400, 500, 1000, 500, 1000, 260,screenWidth, screenHeight);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		plat1.createOBB(matrix);
	glPopMatrix();

	glPushMatrix();
		plat2.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		plat2.createPlatformAndDraw(200, 230, 200, 305, 400, 305, 400, 230, screenWidth, screenHeight);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		plat2.createOBB(matrix);
	glPopMatrix();
	

	//A platform with an NPC on
	glPushMatrix();
		plat3.textureWrapType = 1;
		glTranslatef(-Xdir, -Ydir, 0.0);
		plat3.createPlatformAndDraw(800, 600, 800, 780, 2000, 780, 2000, 600, screenWidth, screenHeight);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		plat3.createOBB(matrix);
	glPopMatrix();

	//A moving platform
	glPushMatrix();
		plat4.textureWrapType = 1;
		glTranslatef(-Xdir + plat4.XPla, -Ydir + plat4.YPla, 0.0);
		plat4.createPlatformAndDraw(1600, 850, 1600, 1030, 2800, 1030, 2800, 850, screenWidth, screenHeight);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		plat4.createOBB(matrix);
	glPopMatrix();

	//The NPC
	glPushMatrix();
		glTranslatef(-Xdir + enemy.XPla, -Ydir + enemy.YPla, 0.0);
		enemy.addPointsandDraw(900, 780, 900, 900, 1000, 900, 1000, 780);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		enemy.createOBB(matrix);
	glPopMatrix();
	enemy.drawOBB();
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

	glRasterPos2f(screenWidth * 2 - 640, screenHeight * 2 - 100);
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

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//player collisions
	//Platform collisions

	//This part is for testing which platforms are colliding with the player
	bool isColliding = false;

	for (vector<Platform*>::iterator it = platforms.begin();
		it != platforms.end(); it++) {

		Platform * platform = *it;

		//For handling collisions
		isColliding = player.boundingBox.SAT2D(platform->boundingBox);

		//if it's colliding with the ground then set the collision status to that object corresponding to that object
		if (isColliding) {
			player.areCollidingPlatform = true;
			player.collisionStatuses.push_back(platform->typeOfCollision(player, dt));
		}
	}

	//so the player is not colliding with anything at all
	if (player.collisionStatuses.empty()) {
		player.areCollidingPlatform = false;
		//so the player can't jump while in mid air
		player.jumpPressed = true;
	}

	//NPC Collisions
	//This part is for testing which platforms are colliding with the player
	isColliding = false;

	for (vector<NPC*>::iterator it = enemys.begin();
		it != enemys.end(); it++) {

		NPC * npc = *it;

		//For handling collisions
		isColliding = player.boundingBox.SAT2D(npc->boundingBox);

		if (isColliding) {
			npc->typeOfCollision(player, dt);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//enemy collisions

	//This part is for testing which platforms are colliding with the player
	isColliding = false;

	for (vector<Platform*>::iterator it = platforms.begin();
		it != platforms.end(); it++) {

		Platform * platform = *it;

		//For handling collisions
		isColliding = enemy.boundingBox.SAT2D(platform->boundingBox);

		//if it's colliding with the ground then set the collision status to that object corresponding to that object
		if (isColliding) {
			enemy.areCollidingPlatform = true;
			enemy.collisionStatuses.push_back(platform->typeOfCollision(enemy, dt));
		}
	}

	//so the player is not colliding with anything at all
	if (enemy.collisionStatuses.empty()) {
		enemy.areCollidingPlatform = false;
	}
}



void reshape(int width, int height)		// Resize the OpenGL window
{
	Xdir = (screenWidth / width)*player.XPla- Xdir;					//Change the X and Y Position of the viewport depending on 
	Ydir = (screenHeight / height)*Ydir;				//the change of the viewport

	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset the current viewport

	glMatrixMode(GL_PROJECTION);						// select the projection matrix stack
	glLoadIdentity();									// reset the top of the projection matrix to an identity matrix

	gluOrtho2D(0,screenWidth*2,0,screenHeight*2);           // set the coordinate system for the window

	glMatrixMode(GL_MODELVIEW);							// Select the modelview matrix stack
	glLoadIdentity();									// Reset the top of the modelview matrix to an identity matrix

	BuildFont((int) round((double) screenHeight/30.0), (int) round((double)screenHeight / 60.0));
}

void rescaleWindow() {

	//For adjusting the viewport when the character is hitting the edge of it
	//For the Xdirection
	//the left edge
	if ((player.XPla-Xdir) < (screenWidth*2.0*0.2f)) {
		//Xdir is the X direction of the viewport to move, 
		Xdir = player.XPla - (screenWidth*2.0*0.2f) + 0.5f*(player.Xspeed+player.oldXspeed)*dt;
	}
	//the right edge
	else if ((player.XPla-Xdir) > (screenWidth*2.0*0.6f)) {
		Xdir = (player.XPla -( screenWidth * 2.0*0.6f) + 0.5f*(player.Xspeed + player.oldXspeed)*dt);
	}
	

	//For the Ydirection
	//the bottom edge
	if (player.YPla < (screenHeight*2.0*0.154f - 0.416f*screenHeight*2.0)) {
		Ydir = player.YPla - (screenHeight*2.0*0.05f - 0.416f*screenHeight*2.0) + 0.5f*(player.Yspeed + player.oldYspeed)*dt;
	}
	//the top edge
	else if (player.YPla > (screenHeight*2.0*0.6f)) {
		Ydir = (player.YPla - screenHeight * 2.0*0.6f + 0.5f*(player.Yspeed + player.oldYspeed)*dt);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

	BuildFont((int)round((double)screenHeight / 30.0), (int)round((double)screenHeight / 60.0));		// Build The Font
	//Only call loadPNG on the background
	background = loadPNG("Sprites/background.png");
	livesIcon = loadPNG("Sprites/astronautStill.png");

	player.loadTexture("Sprites/astronautStill.png");
	enemy.loadTexture("Sprites/alienSprites/alien_predator_mask/predatormask__0000_idle_1.png");
	ground.loadTexture("Sprites/ground2.png");

	plat1.loadTexture("Sprites/platform3by2.png");
	plat2.loadTexture("Sprites/platform3by2.png");
	plat3.loadTexture("Sprites/platform7by2.png");
	plat4.loadTexture("Sprites/platform7by2.png");
}

void processKeys()
{

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
	plat4.updatePlatformMovement(dt);
	player.updatePlayerMovement(dt);
	enemy.updatePlayerMovement(dt);
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
	AllocConsole();
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
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
				LeftPressed = true;
			}
		break;

		case WM_LBUTTONUP:
			{
	            LeftPressed = false;
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



