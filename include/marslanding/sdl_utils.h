#ifndef __SDL_UTILS__
#define __SDL_UTILS__

#include <SDL2/SDL.h>
#include <stdbool.h> 

// Main window 
SDL_Window *window;

// Main screen
SDL_Renderer *screen;
const int SCREEN_WIDTH;  
const int SCREEN_HEIGHT;

// Gamepad 
SDL_Joystick* gamepad;
const Sint16 MAX_JOYSTICK_AXIS_VALUE;
const bool JOY_MANDATORY;

// Timer
struct timer_t
{
    Uint32 current_tick;
    Uint32 previous_tick;
};
struct timer_t timer;

// Initialize SDL, the window and the screen
bool init_sdl();

// Initialize joystick
bool init_joystick();

// Initialize timer
void init_timer();

// Destroy screen + window and quit SDL
void quit_sdl();

#endif
