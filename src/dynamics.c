#include "marslanding/dynamics.h"

#include "marslanding/sdl_utils.h"
#include "marslanding/game.h"

#include <SDL2/SDL.h>
#include <stdlib.h> 
#include <stdio.h>
#include <math.h> 

const double FORWARD_TIME_STEP = 0.01;

// Vehicule parameters
const double DRY_MASS = 1505.0;
const double WET_MASS = 1905.0;
const double ISP = 225.0;
const int NB_THRUSTERS = 6;
const double T_1 = 0.3;
const double T_2 = 0.8;
const double T_bar = 3100.0;
const double PHI = 27.0;

const double MARS_GRAVITY = 3.7114;
const double EARTH_GRAVITY = 9.807;


const double STATE_DIM = 5;
const int PX = 0, PZ  = 1, VX = 2, VZ = 3, M = 4;
const char* STATE_NAMES[] = {"X","Z","VX","VZ","M",NULL};
const char* STATE_UNITS[] = {"m","m","m/s","m/s","kg",NULL};

const double INITIAL_STATE[] = {2000.0, 1500.0, 100.0, -75.0, 1905.0};

struct state_list_t * state_list = NULL;

double current_thrust_x = 0.0;
double current_thrust_z = 0.0;
double current_thrust_norm = 0.0;

double cos_phi = 0.0;
double alpha = 0.0;
double rho_1 = 0.0, rho_2 = 0.0;

bool is_dry = false;
bool is_grounded = false;

// Non-const copy of initial state
double* copy_initial_state()
{
    double *state = malloc(STATE_DIM*sizeof(double));
    if(state == NULL) return NULL;

    for(int i = 0; i < STATE_DIM; i++)
        state[i] = INITIAL_STATE[i];

    return state;
}

// Initialize dynamics parameters
void init_dynamics()
{
    cos_phi = cos(PHI*M_PI/180.0);
    alpha = 1.0/ISP/EARTH_GRAVITY/cos_phi;
    rho_1 = (double)(NB_THRUSTERS)*T_1*T_bar*cos_phi;
    rho_2 = (double)(NB_THRUSTERS)*T_2*T_bar*cos_phi;

    current_thrust_z = MARS_GRAVITY*INITIAL_STATE[M];
    current_thrust_x = 0.0;

    is_dry = (INITIAL_STATE[M] <= DRY_MASS);
    is_grounded = (INITIAL_STATE[PZ] <= 0.0);
    
    compute_thrust();
}

// Initialize linked list of states with initial conditions
void* init_state_list()
{    
    state_list = free_state_list(state_list);
    
    state_list = add_state(NULL,0.0,copy_initial_state());
    
    return state_list;
}

// Integrate dynamics for a fixed step time
double* forward_step(double *state, double step)
{    
    double* new_state = euler(state,step);
    if (new_state == NULL) return NULL;

    // ground impact event
    if (new_state[PZ] <= 0.0) 
    {
        is_grounded = true;
        GAME_OVER = true;
    }

    // dry event
    if (new_state[M] <= DRY_MASS) is_dry = true;

    return new_state;
}

double * euler(double *state, double step)
{
    double* new_state = system_dynamics(state);
    if (new_state == NULL) return NULL;

    for(int i = 0; i < STATE_DIM; i++)
    {
        // Euler step
        new_state[i] *= step;
        new_state[i] += state[i];
    }

    return new_state;
}

// Integrate dynamics for any duration with small steps stored in the global linked list
void forward()
{
    if (!is_grounded)
    {
        // Compute trajectories
        timer.current_tick = SDL_GetTicks();
        double elapsed_time = (double)(timer.current_tick-timer.previous_tick)/1000.0;

        state_list = forward_duration(state_list,elapsed_time);

        timer.previous_tick = timer.current_tick;
        // print_current_state(); 
    }
}

// Integrate dynamics for any duration with small steps stored in a linked list
struct state_list_t * forward_duration(struct state_list_t *state, double duration)
{
    if(duration <= 0.0) return state;
    
    if(duration > FORWARD_TIME_STEP)
    {
        state = add_state(state, // head of list
            state->time+FORWARD_TIME_STEP, // time
            forward_step(state->state,FORWARD_TIME_STEP)); // state

        return forward_duration(state,duration-FORWARD_TIME_STEP);
    }
    else
    {
        state = add_state(state, // head of list
            state->time+duration, // time
            forward_step(state->state,duration)); // state

        return state;
    }
}

// Compute system dynamics
double* system_dynamics(double *state)
{
    if(state == NULL) return NULL;
    
    double* dynamics = malloc(STATE_DIM*sizeof(double));
    if(dynamics == NULL) return NULL;

    dynamics[PX] = state[VX];
    dynamics[PZ] = state[VZ];

    if (state[M] > DRY_MASS && state[PZ] > 0.0)
    {
        dynamics[VX] = current_thrust_x/state[M];
        dynamics[VZ] = -MARS_GRAVITY + current_thrust_z/state[M];
        dynamics[M] = -alpha*current_thrust_norm;
    }
    else if (state[M] <= DRY_MASS && state[PZ] > 0.0)
    {
        dynamics[VX] = 0.0;
        dynamics[VZ] = -MARS_GRAVITY;
        dynamics[M] = 0.0;
    }   
    else
    {
        dynamics[VX] = 0.0;
        dynamics[VZ] = 0.0;
        dynamics[M] = 0.0;
    } 

    return dynamics;
}

struct state_list_t * predict(struct state_list_t *initial_state)
{
    if (initial_state == NULL) return NULL;
    if (initial_state->state == NULL) return NULL;

    struct state_list_t * state = malloc(sizeof(struct state_list_t));
    if (state == NULL) return NULL;
    state->state = malloc(STATE_DIM*sizeof(double));

    state->time = initial_state->time;
    for (int i = 0; i < STATE_DIM; i++)
    {
        state->state[i] = initial_state->state[i];
    }
    state->next = NULL;

    while((state->state[PZ] >= 0.0) && (state->state[M] > DRY_MASS))
    {
        state = add_state(state,state->time+FORWARD_TIME_STEP,euler(state->state,FORWARD_TIME_STEP));        
    }

    return state;
}

void compute_thrust()
{
    if (joy_thrust_x == 0.0 && joy_thrust_z == 0.0)
    {
        current_thrust_x = 0.0;
        current_thrust_z = MARS_GRAVITY*state_list->state[M];
        current_thrust_norm = current_thrust_z;
    }
    else
    {
        current_thrust_norm = rho_1 + (rho_2-rho_1)*joy_thrust_n;
        current_thrust_x = joy_thrust_x*current_thrust_norm;
        current_thrust_z = joy_thrust_z*current_thrust_norm;
    }    
}

void print_current_state()
{    
    printf("Current state (t=%.2fs) \n{\n",state_list->time);
    for(int i = 0; i < STATE_DIM; i++)
    {
        printf("  %s = %.2f %s",STATE_NAMES[i],state_list->state[i],STATE_UNITS[i]);
        if (i < STATE_DIM-1)
            printf("\n");
    }
    printf(" (DRY @ %.2f)\n",DRY_MASS);

    int throttle_level = current_thrust_norm/NB_THRUSTERS/T_bar/cos_phi*100;
    printf("  TX = %.2f N\n",current_thrust_x);
    printf("  TZ = %.2f N\n",current_thrust_z);
    printf("  |T| = %.2f N (%i%%)",current_thrust_norm,throttle_level);

    printf("\n}\n");
}

void print_current_thrust()
{
    int throttle_level = current_thrust_norm/NB_THRUSTERS/T_bar/cos_phi*100;

    printf("Current thrust : Tx = %f, Tz = %f, Tn = %f (%i%%)\n",
        current_thrust_x,current_thrust_z,current_thrust_norm,throttle_level);
}