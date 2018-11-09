#include "shared_code.h"
#include "game.h"
#include <float.h>
 
void (*gameInit_ptr)(GameState *game) = &Init; 
void  (*gameUpdate_ptr)(GameState *game) = &Update; 
void (*gameDraw_ptr)(GameState *game) = &Draw; 

typedef struct
{
    int Width;
    int Height;
} WinDim;

//globals
LPSTR szClassName = "MyClass";
HINSTANCE hInstance;
LRESULT CALLBACK MyWndProc(HWND, UINT, WPARAM, LPARAM);
WINDOWPLACEMENT GlobalWindowPosition = {sizeof(GlobalWindowPosition)};
int64_t GlobalPerfCountFrequency;
GameState gamestate = {0};

//==========================
//	TIMING
//==========================
inline LARGE_INTEGER GetWallClock(void)
{    
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return(result);
}

inline float GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    float result = ((float)(end.QuadPart - start.QuadPart) / (float)GlobalPerfCountFrequency);
    return(result);
}


//==========================
//	DISPLAY
//==========================
void ToggleFullscreen(HWND Window)
{
    //http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
    
    DWORD Style = GetWindowLong(Window, GWL_STYLE);
    if(Style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
        if(GetWindowPlacement(Window, &GlobalWindowPosition) &&
           GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
        {
            SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(Window, HWND_TOP,
                         MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                         MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                         MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(Window, &GlobalWindowPosition);
        SetWindowPos(Window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

WinDim GetWindowDimension(HWND window)
{
    WinDim result;
    
    RECT clientRect;
    GetClientRect(window, &clientRect);
    result.Width = clientRect.right - clientRect.left;
    result.Height = clientRect.bottom - clientRect.top;

    return(result);
}

void DisplayBufferInWindow(GameState *game, HDC deviceContext,HWND hwnd)
{
	//calculate correct letterboxing / pillarboxing
	WinDim Dimensions = GetWindowDimension(hwnd);
	float aspectRatio = (float) RES_X / RES_Y;
	int width = Dimensions.Width;
	int height = (int)(width / aspectRatio + 0.5f);
	
	if (height > Dimensions.Height)
	{
		height = Dimensions.Height;
		width = (int) (height * aspectRatio + 0.5f);
	}
	int x = (Dimensions.Width / 2) - (width / 2);
	int y = (Dimensions.Height / 2) - (height / 2);

    StretchDIBits(deviceContext,
                  x, y, width, height,
                  0, 0, game->pixels.Width, game->pixels.Height,
                  game->pixels.Memory,
                  &game->pixels.Info,
                  DIB_RGB_COLORS, SRCCOPY);
}






int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	
	//invoke Init function
	(*gameInit_ptr)(&gamestate);	
		
	//timing
	LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    UINT DesiredSchedulerMS = 1;
    bool SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);

	//window creation
	WNDCLASS wnd;
    MSG msg;
    HWND hwnd;
 
    hInstance = hInst;
         
    wnd.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    wnd.lpfnWndProc = MyWndProc;
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hInstance = hInstance;
    wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
    wnd.lpszMenuName = NULL;
    wnd.lpszClassName = szClassName;

    if(RegisterClass(&wnd))
    {
        hwnd = CreateWindow(szClassName,
							WINDOW_TITLE,
							WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT,
							CW_USEDEFAULT,      
							CW_USEDEFAULT,
							CW_USEDEFAULT,       
							NULL,            
							NULL,              
							hInstance,
							NULL);               
							
		if(hwnd)
        {
		
	
            HDC deviceContext = GetDC(hwnd);
			
			//timing
			float TargetSecondsPerFrame = 1.0f / 60.0f; 
			LARGE_INTEGER LastCounter = GetWallClock();
			uint64_t LastCycleCount = __rdtsc();
			
            while(!gamestate.RequestQuit)
            {
				
				//invoke Update function
				(*gameUpdate_ptr)(&gamestate); 
				
				if (gamestate.RequestFullScreenToggle)
				{
					ToggleFullscreen(hwnd);
					gamestate.RequestFullScreenToggle = false;
				}
				
				//clear controller data right after update and recalc it
				for (int i = 0; i < 5; i++)	
					memset(&gamestate.controllers[i],0,sizeof(UserInput));
				
				
				UserInput* keyboard = &gamestate.controllers[4];
                while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    if(msg.message == WM_QUIT)
                    {
                        gamestate.RequestQuit = true;
                    }

					//keyboard handling
					if (msg.message == WM_SYSKEYDOWN)
					{
						if (msg.wParam==VK_RETURN) 
						{
							if (msg.hwnd)
								ToggleFullscreen(msg.hwnd);
						}
					}
					
					if (msg.message == WM_KEYUP)
					{
						if (msg.wParam == VK_UP || msg.wParam == 'W')
						{
							keyboard->UpTapped = true;
							keyboard->Up = false;
						}
						if (msg.wParam == VK_DOWN || msg.wParam == 'S')
						{
							keyboard->DownTapped = true;
							keyboard->Down = false;
						}
						if (msg.wParam == VK_LEFT || msg.wParam == 'A')
						{
							keyboard->LeftTapped = true;
							keyboard->Left = false;
						}
						if (msg.wParam == VK_RIGHT || msg.wParam == 'D')
						{
							keyboard->RightTapped = true;
							keyboard->Right = false;
						}
						if (msg.wParam == VK_SPACE)
						{
							keyboard->StartTapped = true;
							keyboard->Start = false;
						}
						if (msg.wParam == VK_ESCAPE)
						{
							keyboard->BackTapped = true;
							keyboard->Back = false;
						}
						if (msg.wParam == 'Z')
						{
							keyboard->Action1Tapped = true;
							keyboard->Action1 = false;
						}
						if (msg.wParam == 'X')
						{
							keyboard->Action2Tapped = true;
							keyboard->Action2 = false;
						}
					}
					
					if (msg.message == WM_KEYDOWN)
					{
						bool altKeyWasDown = (msg.lParam & (1 << 29));
						if((msg.wParam == VK_F4) && altKeyWasDown)
                        {
                            gamestate.RequestQuit = true;
                        }

						if (msg.wParam == VK_UP || msg.wParam == 'W')
						{
							keyboard->Up = true;
						}
						if (msg.wParam == VK_DOWN || msg.wParam == 'S')
						{
							keyboard->Down = true;
						}
						if (msg.wParam == VK_LEFT || msg.wParam == 'A')
						{
							keyboard->Left = true;
						}
						if (msg.wParam == VK_RIGHT || msg.wParam == 'D')
						{
							keyboard->Right = true;
						}
						if (msg.wParam == VK_SPACE)
						{
							keyboard->Start = true;
						}
						if (msg.wParam == VK_ESCAPE)
						{
							keyboard->Back = true;
						}
						if (msg.wParam == 'Z')
						{
							keyboard->Action1 = true;
						}
						if (msg.wParam == 'X')
						{
							keyboard->Action2 = true;
						}
						
					}
                    
                    TranslateMessage(&msg);
					DispatchMessage(&msg);
                }

				
				//invoke Draw function
				(*gameDraw_ptr)(&gamestate); 
				
                DisplayBufferInWindow(&gamestate, deviceContext, hwnd);
				
				LARGE_INTEGER WorkCounter = GetWallClock();
                float WorkSecondsElapsed = GetSecondsElapsed(LastCounter, WorkCounter);
                float SecondsElapsedForFrame = WorkSecondsElapsed;
                if(SecondsElapsedForFrame < TargetSecondsPerFrame)
                {                        
                    if(SleepIsGranular)
                    {
                        DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame -
                                                           SecondsElapsedForFrame));
                        if(SleepMS > 0)
                        {
                            Sleep(SleepMS);
                        }
                    }
                
                    float TestSecondsElapsedForFrame = GetSecondsElapsed(LastCounter, GetWallClock());
                
                    while(SecondsElapsedForFrame < TargetSecondsPerFrame)
                    {                            
                        SecondsElapsedForFrame = GetSecondsElapsed(LastCounter, GetWallClock());
                    }
                }
				
				
				//timing
				LARGE_INTEGER EndCounter = GetWallClock();
                float MSPerFrame = GetSecondsElapsed(LastCounter, EndCounter);   LastCounter = EndCounter;
				
				if (gamestate.ElapsedTimeSeconds + MSPerFrame < DBL_MAX)
				{
					gamestate.ElapsedTimeSeconds += MSPerFrame;
				}
				else
				{
					gamestate.ElapsedTimeSeconds = 0.0;
				}
            }
        }
        else
        {
            MessageBox(NULL, "Unable to create window!", "Error", MB_OK);
			return 0;
        }
    }
	else
	{
		MessageBox(NULL, "Unable to register window class!", "Error", MB_OK);
        return 0;
	}
 
    return msg.wParam;
}
 
LRESULT CALLBACK MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch(msg)
    {
        case WM_CLOSE:
        {
            gamestate.RequestQuit = true;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            gamestate.RequestQuit = true;
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(hwnd, &paint);
            DisplayBufferInWindow(&gamestate, deviceContext, hwnd);
            EndPaint(hwnd, &paint);
        } break;

        default:
        {
            result = DefWindowProc(hwnd, msg, wParam, lParam);
        } break;
    }
    
    return(result);
}