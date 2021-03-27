#ifndef __DYNAMICS__
#define __DYNAMICS__

#include "marslanding/state_list.h"

#include <string.h>

const extern double FORWARD_TIME_STEP;

const extern double DRY_MASS;
const extern double WET_MASS;
const extern double ISP;
const extern int NB_THRUSTERS;
const extern double T_1, T_2, T_bar;
const extern double PHI;

const extern double MARS_GRAVITY;
const extern double EARTH_GRAVITY;

const extern double STATE_DIM;
const extern int PX, PZ, VX, VZ, M;
const extern char* STATE_NAMES[];
const extern char* STATE_UNITS[];

const extern double INITIAL_STATE[];

extern struct state_list_t * state_list;

extern double current_thrust_x;
extern double current_thrust_z;
extern double current_thrust_norm;

extern double cos_phi;
extern double alpha;
extern double rho_1, rho_2;

extern bool is_dry;
extern bool is_grounded;

// Non-const copy of initial state
double* copy_initial_state();

// Initialize linked list of states with initial conditions
void* init_state_list();

// Initialize dynamics parameters
void init_dynamics();

// Integrate dynamics for a small time step
double* forward_step(double *state, double step);

double * euler(double *state, double step);

// Integrate dynamics with small steps and store them in the global linked list
void forward();

// Intergate dynamics for any duration and stores step in a linked list
struct state_list_t * forward_duration(struct state_list_t *state, double duration);

// Compute system dynamics
double* system_dynamics(double *state);

struct state_list_t * predict(struct state_list_t *state);

void compute_thrust();

void print_current_state();

void print_current_thrust();

#endif