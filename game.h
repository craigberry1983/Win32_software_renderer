#include "shared_code.h"
#include "renderer.h"
#include "game_menus.h"

#define HORIZON_HEIGHT 175
#define RIVER_SURFACE_AMOUNT 35

void Init(GameState *game)
{
	ResetGame(game);
}

void Update(GameState *game)
{
	
}


void DrawRiver(GameState *game)
{	
	float r = 0.34f;
	float g = 0.32f;
	float b = 0.22f;
	float mixAmount = 0.85f;
	
	int sineAngle = 0;
	for (int x = 0; x < RES_X; x++)
	{		
		game->OceanPixels[x] = (int)(
			(1.5f * sin(sineAngle * (1.0f / 1500.0f) + (game->ElapsedTimeSeconds * 4.0f))) +
			(1.0f * sin(sineAngle * (1.0f / 45.0f) - (game->ElapsedTimeSeconds * 5.0f))) +	
			(1.0f * sin(sineAngle * (1.0f / 400.0f) - (game->ElapsedTimeSeconds * 7.5f))) / 3);
		
		//draw line
		PlotPixel(game, x, HORIZON_HEIGHT + RIVER_SURFACE_AMOUNT + game->OceanPixels[x], r,g,b);
		
		//draw reflective ocean surface
		float z = 0.0f;
		for (int y = HORIZON_HEIGHT; y < HORIZON_HEIGHT + RIVER_SURFACE_AMOUNT + game->OceanPixels[x]; y++)
		{
			//calculate uv lookup with faked "outward" perspective
			int u = (int)(x + 2.0f * ((RES_X * 0.5f) - x) * z) + game->OceanPixels[x];
			int v = (int)(HORIZON_HEIGHT - (y - (HORIZON_HEIGHT + game->OceanPixels[x])) * 1.7f); //1.7f gets more of the sky in the reflection
			z += 0.0032f;
			
			
			//check bounds of uv lookup are within pixel buffer
			if (u < 0) { u = 0; }
			if (u > RES_X - 1) { u = RES_X - 1; }
			if (v < 0) { v = 0; }
			if (v > HORIZON_HEIGHT - 1) { v = HORIZON_HEIGHT - 1; } //-1?
			
			//uv lookup for reflected color
			uint32_t* uv = (uint32_t*)(((uint8_t*)game->pixels.Memory + game->pixels.Pitch * v)) + u; 
			float surfaceR = (float)(((*uv >> 16) & 255) / 255.0f);
			float surfaceG = (float)(((*uv >> 8) & 255) / 255.0f);
			float surfaceB = (float)(((*uv >> 0) & 255) / 255.0f);
			
			//lerp rgb with water color
			surfaceR = surfaceR + ((mixAmount * (r - surfaceR)));
			surfaceG = surfaceG + ((mixAmount * (g - surfaceG)));
			surfaceB = surfaceB + ((mixAmount * (b - surfaceB)));
			
			PlotPixel(game, x, y, surfaceR, surfaceG, surfaceB);
		}
		//draw underwater (lerp)
		for (int y = HORIZON_HEIGHT + RIVER_SURFACE_AMOUNT + game->OceanPixels[x] + 1; y < RES_Y; y++)
		{
			float underR = r * mixAmount; float r2 = underR * 0.3f;
			float underG = g * mixAmount; float g2 = underG * 0.3f;
			float underB = b * mixAmount; float b2 = underB * 0.3f;
			float distance = (float)(RES_Y - HORIZON_HEIGHT + RIVER_SURFACE_AMOUNT + game->OceanPixels[x] + 2);
			float currentValue = (float)(y - HORIZON_HEIGHT + RIVER_SURFACE_AMOUNT + game->OceanPixels[x] + 2);
			underR = underR + (currentValue / distance) * (r2 - underR);
			underG = underG + (currentValue / distance) * (g2 - underG);
			underB = underB + (currentValue / distance) * (b2 - underB);
			PlotPixel(game, x, y, underR, underG, underB);
		}
		
		sineAngle += 10; 
	}
}

void GameScreen(GameState* game)
{
	
	if (AnyBackTapped(game))
	{
		ChangeScreen(game, PauseScreen);
	}
	
	//clear backbuffer to sky color
	PlotRectangle(game, 0.0f, 0.0f, RES_X, RES_Y, 0.3f, 0.5f, 0.5f);
	
   //draw river bank
	DrawBMP(game, &game->BankBMP, 0, 84, 0, 0, true, 1.0f, false);
	
	//draw boat (for reflection)
	DrawBMP(game, &game->PlayerBMP, 96, 140, 0, 0, true, 1.0f, false);
	
	//draw water
	DrawRiver(game);
	
	//draw boat 
	DrawBMP(game, &game->PlayerBMP, 96, 140, 0, 0, true, 1.0f, false);
	
	//draw instructions
	PlotText(game, 90, 250, 0.34f, 0.32f, 0.22f, FONT_LARGE, "Press ESC to exit");
}

void Draw(GameState *game) 
{ 
	game->CurrentScreen(game);
} 