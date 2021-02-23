#include "state_list.h"
#include <stddef.h>
#include <stdlib.h> 

long long unsigned int state_list_length = 0;

// Add a state to a linked list of states
struct state_list_t * add_state(struct state_list_t *list, double time, double *state)
{
    if(state == NULL) return NULL;
    
    struct state_list_t *new_state = malloc(sizeof(struct state_list_t));
    if(new_state == NULL) return NULL;

    // Filling the state
    new_state->time = time;
    new_state->state = state;
    // Linking
    new_state->next = list;

    return new_state;
}

// Free memory for a list
struct state_list_t* free_state_list(struct state_list_t* list)
{
    if(list == NULL) return NULL;
        
    struct state_list_t* tmp;
    
    while(list != NULL)
    {
        tmp = list->next;
        if (list->state != NULL) free(list->state);
        free(list);
        list = tmp;
    }

    return NULL;
}

// Free all samples before a certain time
bool shorten_state_list(struct state_list_t* list, double min_time)
{
    if(list == NULL) return false;
    
    if(list->time >= min_time)
    {
        if(shorten_state_list(list->next,min_time))
        {
            list->next = NULL;
        }

        return false;
    }
    else
    {
        free_state_list(list);
        return true;
    }    
}