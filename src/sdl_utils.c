#include <stdio.h>

#include "marslanding/sdl_utils.h"

// Main window 
SDL_Window *window = NULL;

// Main screen
SDL_Renderer *screen = NULL;
const int SCREEN_WIDTH = 1200; // 1366  640;  
const int SCREEN_HEIGHT = 650;// 650   480;

// Gamepad 
SDL_Joystick* gamepad = NULL;
const Sint16 MAX_JOYSTICK_AXIS_VALUE = 32767; 
const bool JOY_MANDATORY = true;

struct timer_t timer;

// Initialize SDL, the window and the screen
bool init_sdl()
{
    // Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

    // Create window
    window = SDL_CreateWindow("Manual Mars Landing", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN);

    if(window == NULL)
    {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    // Create screen renderer for window
    screen = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // screen = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if(screen == NULL)
    {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    SDL_SetRenderDrawBlendMode(screen, SDL_BLENDMODE_BLEND);

    // Initial joystick if wanted
    if(JOY_MANDATORY)
    {
        return init_joystick();
    }

    return true;
}

// Initialize joystick
bool init_joystick()
{
    // Check for joysticks
    if(SDL_NumJoysticks() < 1)
    {
        printf("Warning: No joysticks connected!\n");
        return false;
    }

    //Load joystick
    gamepad = SDL_JoystickOpen(0);
    if(gamepad == NULL)
    {
        printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

// Initialize timer
void init_timer()
{
    timer.current_tick = SDL_GetTicks();
    timer.previous_tick = timer.current_tick;
}

// Destroy screen + window and quit SDL
void quit_sdl()
{
    // Close game controller
    SDL_JoystickClose(gamepad);
    gamepad = NULL;
    
    // Destroy window	
	SDL_DestroyRenderer(screen);
	SDL_DestroyWindow(window);
	window = NULL;
	screen = NULL;

	// Quit SDL subsystems
	SDL_Quit();
}