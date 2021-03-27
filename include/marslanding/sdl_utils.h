#ifndef __SDL_UTILS__
#define __SDL_UTILS__

#include <SDL2/SDL.h>
#include <stdbool.h> 

// Main window 
extern SDL_Window *window;

// Main screen
extern SDL_Renderer *screen;
const extern int SCREEN_WIDTH;  
const extern int SCREEN_HEIGHT;

// Gamepad 
extern SDL_Joystick* gamepad;
const extern Sint16 MAX_JOYSTICK_AXIS_VALUE;
const extern bool JOY_MANDATORY;

// Timer
struct timer_t
{
    Uint32 current_tick;
    Uint32 previous_tick;
};
extern struct timer_t timer;

// Initialize SDL, the window and the screen
bool init_sdl();

// Initialize joystick
bool init_joystick();

// Initialize timer
void init_timer();

// Destroy screen + window and quit SDL
void quit_sdl();

#endif
