//Including Libraries and my Rectangle Header
#define _USE_MATH_DEFINES
#include <HAPI_lib.h>
#include "Rectangle.h"
#include <cmath>
using namespace HAPISPACE;

//Function to rotate about a point
//Takes the X and Y of the Fulcrum, the position to be rotated and the Angle
float RotateAboutPoint(float posX, float posY, float AposX, float AposY, float Angle, bool x)
{
	float TposX;
	float TposY;
	float sinAngle;
	float cosAngle;
	sinAngle = sin(Angle);
	cosAngle = cos(Angle);
	TposX = posX - AposX;
	TposY = posY - AposY;

	if (x == true)
	{
		float RposX;
		float FposX;
		RposX = (TposX * cosAngle) - (TposY * sinAngle);
		FposX = RposX + AposX;
		return FposX;//Outputs the Final Position of the X
	}

	else if (x == false)
	{
		float RposY;
		float FposY;
		RposY = (TposX * sinAngle) + (TposY * cosAngle);
		FposY = RposY + AposY;
		return FposY;//Outputs the Final Position of the Y
	}
}

//Function to Blit the textures using clipping incase the spaceship goes of screen 
void Blit(BYTE* screen, BYTE* texture, const Rectangle& screenRect, int posx, int posy, int m_tWidth, int m_tHeight)
{
	Rectangle texRect(0, m_tWidth, 0, m_tHeight);
	Rectangle clippedRect(texRect);
	clippedRect.Translate(posx, posy);
	clippedRect.ClipTo(screenRect);
	clippedRect.Translate(-posx, -posy);

	if (posx < 0)
		posx = 0;
	if (posy < 0)
		posy = 0;

	BYTE* tempTexture{ texture + (size_t)clippedRect.left + clippedRect.top * texRect.Width() * 4 };
	BYTE* tempScreen{ screen + ((size_t)posx + posy * screenRect.Width()) * 4 };
	int increm = (screenRect.Width() - clippedRect.Width()) * 4;
	for (int y = 0; y < clippedRect.Height(); y++)
	{
		for (int x = 0; x < clippedRect.Width(); x++)
		{
			if (tempTexture[3] == 255)
			{
				tempScreen[0] = tempTexture[0];
				tempScreen[1] = tempTexture[1];
				tempScreen[2] = tempTexture[2];
			}
			else if (tempTexture[3] > 0)
			{
				float mod = tempTexture[3] / 255.0f;
				tempScreen[0] = tempTexture[0] * mod + tempScreen[0] * (1.0 - mod);
				tempScreen[1] = tempTexture[1] * mod + tempScreen[1] * (1.0 - mod);
				tempScreen[2] = tempTexture[2] * mod + tempScreen[2] * (1.0 - mod);
			}
			tempTexture += 4;
			tempScreen += 4;
		}
		tempScreen += increm;
		tempTexture += ((size_t)m_tWidth - clippedRect.Width()) * 4;
	}
}

void DrawDiagonalLine(BYTE* screen, int screenWidth, int screenheight, float startx, float starty, float endx, float endy, int red, int green, int blue)
{
	float lengthx = endx - startx;
	float lengthy = endy - starty;
	float length = sqrt((lengthx * lengthx) + (lengthy * lengthy));
	float calcX = startx;
	float calcy = starty;

	for (float i = 0; i < 1; i += 1.0f / length / 2)
	{
		calcX = (1 - i) * startx + i * endx;
		calcy = (1 - i) * starty + i * endy;
		int drawX = calcX;
		int drawY = calcy;

		if (drawY >= 0 && drawY < screenheight && drawX >= 0 && drawX < screenWidth) {
			screen[drawY * screenWidth * 4 + drawX * 4] = red;
			screen[drawY * screenWidth * 4 + drawX * 4 + 1] = green;
			screen[drawY * screenWidth * 4 + drawX * 4 + 2] = blue;
		}
	}
}

//This is a function to clear the screen
void ClearScreen(BYTE* screen, int screenwidth, int screenheight)
{
	memset(screen, 0, (size_t)screenwidth * screenheight * 4);
}

//This is to define the Screen Boundries
void DrawBox(BYTE* screen, int screenwidth, int screenheight, float left, float top, float right, float bottom)
{
	DrawDiagonalLine(screen, screenwidth, screenheight, left, top, right, top, 0, 255, 0);
	DrawDiagonalLine(screen, screenwidth, screenheight, left, top, left, bottom, 0 , 255, 0);
	DrawDiagonalLine(screen, screenwidth, screenheight, right, top, right, bottom, 0, 255, 0);
	DrawDiagonalLine(screen, screenwidth, screenheight, left, bottom, right, bottom, 255, 0, 0);
}

void HAPI_Main()
{
	//Defining Screen & game boundries
	int screenwidth{ 1820 };
	int screenheight{ 900 };
	Rectangle screenRect(0, screenwidth, 0, screenheight);
	float left = 50;
	float right = screenwidth - 50;
	float top = 50;
	float bottom = screenheight - 50;

	//Gameplay, Position & Physics values declared
	float PosX = 50;
	float PosY = -900;
	float L_VelocityY = 0.7;
	float L_VelocityX = 0.7;
	int round = 1;
	float difficulty = 300;
	srand(time(0));
	float safeZone_L = ((rand() % 1420) + 50);
	float safeZone_R = safeZone_L + difficulty;
	int tempx;
	int tempy;
	float sideThrust;
	float up_downThrust;

	//Bool Checks for parameters
	bool OnTheGround = false;
	float angle = 0;
	bool dead = false;
	bool win = false;

	//Declaring Variables for HAPI
	const HAPI_TKeyboardData& KeyData = HAPI.GetKeyboardData();
	HAPI_TColour textGreen{ HAPI_TColour::GREEN };
	HAPI_TColour outlineGreen{ HAPI_TColour::GREEN };
	HAPI_TColour textRed{ HAPI_TColour::RED };
	HAPI_TColour outlineRed{ HAPI_TColour::RED };

	//The different point of the Lunar Lander
	float lander_leftX;
	float lander_leftY;
	float lander_rightX;
	float lander_rightY;
	float lander_topX;
	float lander_topY;
	float lander_bottomX;
	float lander_bottomY;
	
	//Different Strings for rendering text for the HUD 
	std::string String_Round;
	std::string String_PosX;
	std::string String_Velocity;
	std::string String_Angle;
	std::string String_Altitude;
	
	//The Values to be Converted into strings
	int StrPosX;
	int Velocity_S;
	int angle_S;
	int altitude;

	//Loading the textures
	BYTE* texture{ nullptr };
	int m_tWidth, m_tHeight;
	if (!HAPI.LoadTexture("Data\\playerSprite.tga", &texture, m_tWidth, m_tHeight))
	{
		HAPI.UserMessage("Missing player texture", "Error");
		return;
	}

	//Initialising HAPI and Main game loop
	if (!HAPI.Initialise(screenwidth, screenheight, "Lunar Lander"))
		return;

	//The Main Update Loop for the Program
	while (HAPI.Update()) 
	{
		//FrameRater<1000> fr;
		BYTE* screen = HAPI.GetScreenPointer();
		HAPI.SetShowFPS(true);
		ClearScreen(screen, screenwidth, screenheight);

		HAPI.RenderText(60, 50, textGreen, outlineGreen, 1, "Velocity: (ft/sec)", 50);
		HAPI.RenderText(430, 50, textGreen, outlineGreen, 1, "Angle:", 50);
		HAPI.RenderText(600, 50, textGreen, outlineGreen, 1, "Altitude: (ft)", 50);
		HAPI.RenderText(900, 50, textGreen, outlineGreen, 1, "Round:", 50);
		HAPI.RenderText(1200, 50, textGreen, outlineGreen, 1, "X position:", 50);

		//Update the position using the Velocity
		PosY += L_VelocityY;
		PosX += L_VelocityX;
		lander_leftX = PosX - 30;
		lander_leftY = PosY;
		lander_rightX = PosX + 30;
		lander_rightY = PosY;
		lander_topX = PosX;
		lander_topY = PosY - 30;
		lander_bottomX = PosX;
		lander_bottomY = PosY + 30;

		//Check If the Player Is Dead
		if (dead == true)
		{
			HAPI.RenderText(screenwidth / 2 - 170, screenheight / 2 - 150, textGreen, outlineGreen, 1, "Landing Failed", 50);
			//Position and Physics Reseting declaring
			difficulty = 300;
			PosX = 600;
			PosY = -900;
			L_VelocityY = 0.3;
			L_VelocityX = 0.3;
			OnTheGround = false;
			angle = -0.1;
			round = 1;
			if (KeyData.scanCode['X']) //Wait for the Player to Reset
			{
				dead = false;
				safeZone_L = ((rand() % 1420) + 50);
				safeZone_R = safeZone_L + difficulty;
			}
		}
		
		//Check If the Win is true
		if (win == true)
		{
			//Draw the Win Tagline to the screen
			HAPI.RenderText(screenwidth / 2 - 170, screenheight / 2 - 150, textGreen, outlineGreen, 1, "Landing Passed", 50);

			//Position and Physics Reseting declaring
			if (difficulty < 40)
			{
				difficulty = 40;
			}
			PosX = 600;
			PosY = -900;
			L_VelocityY = 0.3;
			L_VelocityX = 0.3;
			OnTheGround = false;
			angle = -0.1;
			if (KeyData.scanCode['X']) //Wait to Reset
			{
				win = false;
				safeZone_L = ((rand() % 1420) + 50);
				safeZone_R = safeZone_L + difficulty;
			}
		}

		//Check if it has landed and if it has wether it was safe/dead
		if (OnTheGround == false && dead == false && win == false)
		{
			L_VelocityY += 0.001;
		}
	
		//If the SpaceShuttle is on the Ground then No Velocity
		if (OnTheGround == true)
		{
			L_VelocityY = 0;
			L_VelocityX = 0;
		}

		//Drawing the Round number to the Screen
		String_Round = std::to_string(round);
		HAPI.RenderText(900, 100, textGreen, outlineGreen, 1, String_Round, 50);
		
		//Drawing the XPosition to the Screen
		StrPosX = PosX;
		String_PosX = std::to_string(StrPosX);
		HAPI.RenderText(1200, 100, textGreen, outlineGreen, 1, String_PosX, 50);

		//Checking if the SpaceShuttle is on the ground and whether its a loss or a win
		if (PosY >= (bottom - 30))
		{
			PosY = bottom - 30;
			OnTheGround = true;
			if (L_VelocityY > 0.3 || PosX > safeZone_R || PosX < safeZone_L) //|| angle > -0.5 && angle < 0.5)
			{
				dead = true;
			}
			else
			{
				difficulty = difficulty - 30;
				round += 1;
				win = true;
			}
		}

		//Checking if the SpaceShuttle goes of the edge then its a loss
		if (PosX > right || PosX < left)
		{
			dead = true;
		}

		//Print the speed of the Velocity
		Velocity_S = L_VelocityY * 1000;
		String_Velocity = std::to_string(Velocity_S);
		if (L_VelocityY > 0.3) 
		{
			HAPI.RenderText(100, 100, textRed, outlineRed, 1, String_Velocity, 50);
			HAPI.RenderText(100, 150, textRed, outlineRed, 1, "Too Fast", 50);
		}
		else if (L_VelocityY < 0.3)
		{
			HAPI.RenderText(100, 100, textGreen, outlineGreen, 1, String_Velocity, 50);
		}

		//Print the radians of the Angle
		angle_S = angle * 10;
		String_Angle = std::to_string(angle_S);
		if (angle > 0.5 || angle < -0.5)
		{
			HAPI.RenderText(430, 100, textRed, outlineRed, 1, String_Angle, 50);
		}
		else
		{
			HAPI.RenderText(430, 100, textGreen, outlineGreen, 1, String_Angle, 50);
		}
		
		//Print he altitude and the tag line for it
		altitude = ((bottom - 30) - PosY) * 3;
		String_Altitude = std::to_string(altitude);
		HAPI.RenderText(600, 100, textGreen, outlineGreen, 1, String_Altitude, 50);

		//Handling the Rotation of the Player
		tempx = lander_rightX;
		tempy = lander_rightY;
		lander_rightX = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, true);
		lander_rightY = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, false);
		tempx = lander_topX;
		tempy = lander_topY;
		lander_topX = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, true);
		lander_topY = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, false);
		tempx = lander_leftX;
		tempy = lander_leftY;
		lander_leftX = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, true);
		lander_leftY = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, false);
		tempx = lander_bottomX;
		tempy = lander_bottomY;
		lander_bottomX = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, true);
		lander_bottomY = RotateAboutPoint(tempx, tempy, PosX, PosY, angle, false);

		//Drawing the lander and game boundries
		Blit(screen, texture, screenRect, PosX - (m_tWidth / 2), PosY - (m_tHeight / 2), m_tWidth, m_tHeight);
		DrawDiagonalLine(screen, screenwidth, screenheight, lander_leftX, lander_leftY, lander_rightX, lander_rightY, 255 , 255, 255);
		DrawDiagonalLine(screen, screenwidth, screenheight, lander_topX, lander_topY, lander_bottomX, lander_bottomY, 255, 255, 255);
		DrawDiagonalLine(screen, screenwidth, screenheight, lander_topX, lander_topY, lander_rightX, lander_rightY, 255, 255, 255);
		DrawDiagonalLine(screen, screenwidth, screenheight, lander_topX, lander_topY, lander_leftX, lander_leftY, 255, 255, 255);
		DrawBox(screen, screenwidth, screenheight, left, top, right, bottom);
		DrawDiagonalLine(screen, screenwidth, screenheight, safeZone_L, bottom, safeZone_R, bottom, 0, 255, 0);
		DrawDiagonalLine(screen, screenwidth, screenheight, safeZone_L, bottom - 1, safeZone_R, bottom - 1, 0, 255, 0);

		//Working out how much thrust should be applied on the x/y axis
		sideThrust = (PosX - lander_bottomX) * 0.0001;
		up_downThrust = (PosY - lander_bottomY) * 0.0001;

		//Handling Player Inputs
		if (dead == false && win == false)
		{
			if (KeyData.scanCode['W'])
			{
				L_VelocityY += up_downThrust;
				L_VelocityX += sideThrust;

				OnTheGround = false;
			}
			if (KeyData.scanCode['A'])
			{
				angle -= 0.01;
			}
			if (KeyData.scanCode['D'])
			{
				angle += 0.01;
			}
		}
	}
}

