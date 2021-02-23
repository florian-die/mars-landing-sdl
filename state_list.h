#ifndef __STATE_LIST__
#define __STATE_LIST__

#include <stdbool.h> 

// Linked list of states
struct state_list_t
{
    double time;
    double *state;
    struct state_list_t *next;
};

// Add a state to a linked list of states
struct state_list_t* add_state(struct state_list_t* list, double time, double* state);

// Free memory for a list
struct state_list_t* free_state_list(struct state_list_t* list);

// Free all samples before a certain time
bool shorten_state_list(struct state_list_t* list, double min_time);

#endif