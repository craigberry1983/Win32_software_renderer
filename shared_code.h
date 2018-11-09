#ifndef SHARED_CODE_H
#define SHARED_CODE_H

#define WINDOW_TITLE "Win32 software renderer"
#define RES_X 320 
#define RES_Y 320 
#define FULLSCREEN 1


#include <windows.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
typedef enum { false, true } bool;

typedef struct 
{
	int32_t Width;
	int32_t Height;
	uint32_t FrameSizePixelsX;
	uint32_t FrameSizePixelsY;
	uint32_t* Pixels;
} LoadedBMP;


typedef struct
{
	uint32_t ContentSize;
	void* Content;
} FileData;

void PlatformFreeFileMemory(void* memory)
{
	if(memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

FileData PlatformReadEntireFile(char* filename)
{
	FileData result = {0};
    
    HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if(GetFileSizeEx(fileHandle, &fileSize))
        {
            uint32_t fileSize32 = (uint32_t)(fileSize.QuadPart);
            result.Content = VirtualAlloc(0, fileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(result.Content)
            {
                DWORD bytesRead;
                if(ReadFile(fileHandle, result.Content, fileSize32, &bytesRead, 0) && (fileSize32 == bytesRead))
                {
                    result.ContentSize = fileSize32;
                }
                else
                {                    
                    PlatformFreeFileMemory(result.Content);
                    result.Content = 0;
                }
            }
        }

        CloseHandle(fileHandle);
    }

    return result;
}

	

typedef struct
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
} PixelBuffer;

typedef struct 
{
	
	bool Up;      bool UpTapped;     
	bool Down;	  bool DownTapped;	
	bool Left; 	  bool LeftTapped; 	
	bool Right;   bool RightTapped;  
	bool Action1; bool Action1Tapped;
	bool Action2; bool Action2Tapped;
	bool Start;   bool StartTapped;  
	bool Back;    bool BackTapped;
	int MouseX;
	int MouseY;
}UserInput;


typedef struct game_state GameState;
#pragma pack(push, 1)
struct game_state
{
	bool RequestFullScreenToggle;
	bool RequestQuit;
		
	//4 gamepads and a keyboard are all the possible controllers
	UserInput controllers[5];
	double ElapsedTimeSeconds;
	PixelBuffer pixels;
	int OceanPixels[RES_X];
	int MenuSelection;
	void (*CurrentScreen)(GameState *game);	
	
	LoadedBMP PlayerBMP;
	LoadedBMP BankBMP;
	LoadedBMP CloudsBMP;

};
#pragma pack(pop) 

void ResizePixelBuffer(GameState *game, int Width, int Height)
{
    if(game->pixels.Memory)
    {
        VirtualFree(game->pixels.Memory, 0, MEM_RELEASE);
    }

    game->pixels.Width = Width;
    game->pixels.Height = Height;

    int BytesPerPixel = 4;

    game->pixels.Info.bmiHeader.biSize = sizeof(game->pixels.Info.bmiHeader);
    game->pixels.Info.bmiHeader.biWidth = game->pixels.Width;
    game->pixels.Info.bmiHeader.biHeight = -game->pixels.Height;
    game->pixels.Info.bmiHeader.biPlanes = 1;
    game->pixels.Info.bmiHeader.biBitCount = 32;
    game->pixels.Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (game->pixels.Width*game->pixels.Height)*BytesPerPixel;
    game->pixels.Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    game->pixels.Pitch = game->pixels.Width*BytesPerPixel;
}



#endif