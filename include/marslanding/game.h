#ifndef __GAME__
#define __GAME__

#include <SDL2/SDL.h>
#include <stdbool.h> 

// Game state
bool extern GAME_PAUSED;
bool extern GAME_OVER;
bool extern QUIT;

bool extern PREDICT;

double extern joy_thrust_x;
double extern joy_thrust_z;
double extern joy_thrust_n;

// Generic SDL Event for PollEvent loop
extern SDL_Event event;

int init_game();

void print_start_ascii();

void print_start_help();

void loop_game();

void handle_events();

void handle_joy_axis();

void saturate(double *x, double min, double max);

void normalize(double *x, double *z);

void handle_joy_buttons();

void render_screen();

void render_pause();

void quit_game();



#endif