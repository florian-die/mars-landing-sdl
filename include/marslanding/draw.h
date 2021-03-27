#ifndef __DRAW__
#define __DRAW__

#include <stdbool.h> 

#include "marslanding/state_list.h"

const  extern int WINDOW_MARGIN;

const extern int SCENE_X, SCENE_Y, SCENE_WIDTH, SCENE_HEIGHT;
const extern double SCENE_MAX_Z, SCENE_MIN_X, SCENE_MAX_X;

extern int scene_x, scene_y, scene_width, scene_height;
double extern scene_delta_x, scene_delta_z;

const extern int GROUND_R, GROUND_G, GROUND_B, GROUND_A;

const extern int TRAJ_R, TRAJ_G, TRAJ_B, TRAJ_A;

const extern double VELOCITY_DRAW_FACTOR;
const extern double THRUST_DRAW_FACTOR;

const extern int SQUARE_WIDTH;

void init_scene();

void draw_scene();

void draw_scene_frame();

void draw_frame(int x, int y, int w, int h);

void draw_ground();

void draw_initial_state();

void draw_square(double px, double pz, int w);

void draw_initial_position();

void draw_initial_velocity();

void draw_arrow(double x, double y, double u, double v, double k);

void draw_objective();

void draw_trajectory();

void draw_state_list(struct state_list_t * list);

void draw_predicted_trajectory();

void scene_coordinates(double px, double pz, int *x, int *y, bool *out);

void draw_current_state();

void draw_current_position();

void draw_current_velocity();

void draw_current_thrust();

void draw_gravity();

void draw_mass();

void draw_mass_frame();

void draw_all();

#endif