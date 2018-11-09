#ifndef GAME_MENUS_H 
#define GAME_MENUS_H

#include "shared_code.h"
#include "renderer.h"

//function prototypes
void IntroScreen(GameState *game);
void StartScreen(GameState *game);
void MenuScreen(GameState *game);
void OptionsScreen(GameState *game);
void GameScreen(GameState *game);
void WinScreen(GameState *game);
void PauseScreen(GameState *game);
void QuitScreen(GameState *game);
void ExitScreen(GameState *game);

bool AnyUpTapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].UpTapped) return true;
	}
	return false;
}

bool AnyDownTapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].DownTapped) return true;
	}
	return false;
}

bool AnyLeftTapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].LeftTapped) return true;
	}
	return false;
}

bool AnyRightTapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].RightTapped) return true;
	}
	return false;
}

bool AnyAction1Tapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].Action1Tapped) return true;
	}
	return false;
}

bool AnyAction2Tapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].Action2Tapped) return true;
	}
	return false;
}

bool AnyStartTapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].StartTapped) return true;
	}
	return false;
}

bool AnyBackTapped(GameState *game)
{
	for (int i = 0; i < 5; i++)
	{
		if (game->controllers[i].BackTapped) return true;
	}
	return false;
}

bool AnyButtonTapped(GameState *game)
{
	return (AnyAction1Tapped(game) || AnyAction2Tapped(game) || AnyStartTapped(game) || AnyBackTapped(game));
}
	
void ChangeScreen(GameState *game, void (*newScreen)(GameState*))
{
	game->MenuSelection = 0;
	game->CurrentScreen = newScreen;
}

void ResetGame(GameState *game)
{
	memset(game,0,sizeof(GameState));
	for (int i = 0; i < 5; i++)	
		memset(&game->controllers[i],0,sizeof(UserInput));
	
	ResizePixelBuffer(game, RES_X, RES_Y);
	
	ChangeScreen(game, MenuScreen);

	//load bitmaps
	game->PlayerBMP = LoadBMP("boat.bmp", 1, 1);
	game->BankBMP = LoadBMP("bank.bmp", 1, 1);
	game->CloudsBMP = LoadBMP("clouds.bmp", 1, 1);	
}



void DoMenuLogic(GameState *game, int totalMenuItems, bool menuHorizontal)
{	
	int diff = 0;
	if ((!menuHorizontal && AnyUpTapped(game)) || (menuHorizontal && AnyLeftTapped(game))) { diff = -1; }
	if ((!menuHorizontal && AnyDownTapped(game)) || (menuHorizontal && AnyRightTapped(game))) { diff = 1; }
	game->MenuSelection = (game->MenuSelection + totalMenuItems + diff) % totalMenuItems;
}

bool DoMenuItem(GameState *game, int index, char* txt, int x, int y)
{
	if (AnyStartTapped(game) && index == game->MenuSelection) { return true; }
	float rgb = 0.0;
	if (game->MenuSelection == index)
	{
		//pulse between 0.0f and 0.2f at a speed of 6.0f
		rgb = (0.3f - 0.0f)/2.0f * ((float)sin(game->ElapsedTimeSeconds * 6.0f) - 1.0f) + 1.0f;
	}
	
	PlotText(game, x, y, 0.7f + rgb, 0.7f + rgb, 0.7f + rgb, FONT_LARGE, txt);
	return false;
}

void MenuScreen(GameState *game)
{
	
	PlotRectangle(game, 0.0f, 0.0f, RES_X, RES_Y, 0.3f, 0.3f, 0.3f);
	PlotText(game, 124, 90, 1.0f,1.0f,1.0f, FONT_LARGE, "Main Menu");

	DoMenuLogic(game, 3, false);	
	if (DoMenuItem(game, 0, "Start Game", 120, 140)) { ChangeScreen(game, GameScreen); }
	if (DoMenuItem(game, 1, "Options", 132, 160)) { ChangeScreen(game, OptionsScreen); }
	if (DoMenuItem(game, 2, "Exit", 144, 180)) { ChangeScreen(game, ExitScreen); }
	
	PlotText(game, 30, 250, 1.0f, 1.0f, 1.0f, FONT_MEDIUM, "Controls: arrow keys, spacebar and escape key");
}

void OptionsScreen(GameState *game)
{
	
	PlotRectangle(game, 0.0f, 0.0f, RES_X, RES_Y, 0.3f, 0.3f, 0.3f);
	
	//display an array of sound/video/control options etc..
	PlotText(game, 104, 90, 1.0f,1.0f,1.0f, FONT_LARGE, "Options Screen");
	
	DoMenuLogic(game, 2, false);	
	if (DoMenuItem(game, 0, "Switch Fullscreen", 92, 140)) { game->RequestFullScreenToggle = true; }
	if (DoMenuItem(game, 1, "Back", 144, 160)) { ChangeScreen(game, MenuScreen); }
	
	PlotText(game, 30, 250, 1.0f, 1.0f, 1.0f, FONT_MEDIUM, "Controls: arrow keys, spacebar and escape key");
}

void PauseScreen(GameState *game)
{
	
	PlotRectangle(game, 0.0f, 0.0f, RES_X, RES_Y, 0.3f, 0.3f, 0.3f);
	
	PlotText(game, 128, 90, 1.0f,1.0f,1.0f, FONT_LARGE, "*Paused*"); 
	
	DoMenuLogic(game, 2, false);	
	if (DoMenuItem(game, 0, "Back To Game", 112, 140)) { ChangeScreen(game, GameScreen); }
	if (DoMenuItem(game, 1, "Quit Game", 124, 160)) { ChangeScreen(game, QuitScreen); }
	
	PlotText(game, 30, 250, 1.0f, 1.0f, 1.0f, FONT_MEDIUM, "Controls: arrow keys, spacebar and escape key");
}

void QuitScreen(GameState *game)
{
	
	PlotRectangle(game, 0.0f, 0.0f, RES_X, RES_Y, 0.3f, 0.3f, 0.3f);
	
	PlotText(game, 72, 140, 1.0f,1.0f,1.0f, FONT_LARGE, "Really Quit This Game?"); 

	DoMenuLogic(game, 2, true);	
	if (DoMenuItem(game, 0, "No", 128, 160)) { ChangeScreen(game, PauseScreen); }
	if (DoMenuItem(game, 1, "Yes", 152, 160)) { ResetGame(game); }
	
	PlotText(game, 30, 250, 1.0f, 1.0f, 1.0f, FONT_MEDIUM, "Controls: arrow keys, spacebar and escape key");
}

void ExitScreen(GameState *game)
{
	
	PlotRectangle(game, 0.0f, 0.0f, RES_X, RES_Y, 0.3f, 0.3f, 0.3f);
	
	PlotText(game, 96, 140, 1.0f,1.0f,1.0f, FONT_LARGE, "Exit To Windows?"); 
	
	DoMenuLogic(game, 2, true);	
	if (DoMenuItem(game, 0, "No", 128, 160)) { ChangeScreen(game, MenuScreen); }
	if (DoMenuItem(game, 1, "Yes", 152, 160)) { game->RequestQuit = true; }
	
	PlotText(game, 30, 250, 1.0f, 1.0f, 1.0f, FONT_MEDIUM, "Controls: arrow keys, spacebar and escape key");

}


#endif