#include "marslanding/draw.h"

#include "marslanding/sdl_utils.h"
#include "marslanding/dynamics.h"
#include "marslanding/state_list.h"
#include "marslanding/game.h"

#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdlib.h> 

const int WINDOW_MARGIN = 10;

const int SCENE_X = 0, SCENE_Y = 0; // in %
const int SCENE_WIDTH = 100, SCENE_HEIGHT = 33; // in %
const int SCENE_TOP_MARGIN = 20; // in px

const double SCENE_MIN_Z = -500.0; // in m
const double SCENE_MAX_Z = 2000.0; // in m
const double SCENE_MIN_X = -500.0; // in m
const double SCENE_MAX_X = 4500.0; // in m

double scene_delta_x = 0;
double scene_delta_z = 0;

int scene_x  = 0, scene_y = 0; // in px
int scene_width = 0, scene_height = 0; // in px

const int GROUND_R = 0xFF, GROUND_G = 0x8C, GROUND_B = 0x00, GROUND_A = 0xFF;

const int TRAJ_R = 0xFF, TRAJ_G = 0xFF, TRAJ_B = 0xFF, TRAJ_A = 0xFF;

const double VELOCITY_DRAW_FACTOR = 3.0;
const double THRUST_DRAW_FACTOR = 0.03;

const int SQUARE_WIDTH = 7;

void init_scene()
{
    scene_x = WINDOW_MARGIN + (SCENE_X*SCREEN_WIDTH)/100;
    scene_y = WINDOW_MARGIN + (SCENE_Y*SCREEN_HEIGHT)/100;

    scene_delta_x = SCENE_MAX_X-SCENE_MIN_X;
    scene_delta_z = SCENE_MAX_Z-SCENE_MIN_Z;

    scene_width = SCENE_WIDTH*(SCREEN_WIDTH-2*WINDOW_MARGIN)/100;
    scene_height = (double) (scene_delta_z/scene_delta_x)* (double) (SCREEN_WIDTH-2*WINDOW_MARGIN);

    // printf("Scene init : x=%i, y=%i, w=%i, h=%i\n",scene_x,scene_y,scene_width,scene_height);
}

void scene_coordinates(double px, double pz, int *x, int *y, bool *out)
{
    *out = false;

    *x = scene_x + scene_width*(px-SCENE_MIN_X)/scene_delta_x;
    *y = scene_y + scene_height*(SCENE_MAX_Z-pz)/scene_delta_z;

    if (*x < scene_x) { *x = scene_x; *out = true; }
    if (*x > scene_x + scene_width) { *x = scene_x + scene_width; *out = true; }
    if (*y < scene_y) { *y = scene_y; *out = true; } 
    if (*y > scene_y + scene_height) { *y = scene_y + scene_height; *out = true; }
}

void draw_scene()
{
    draw_ground();
    draw_objective();

    draw_scene_frame();

    draw_predicted_trajectory();
    draw_trajectory();
    
    draw_initial_state();
    draw_current_state();    
}

void draw_scene_frame()
{
    SDL_SetRenderDrawColor(screen, 0x00, 0x00, 0x00, 0xFF);
    draw_frame(scene_x, scene_y, scene_width, scene_height);
}

void draw_frame(int x, int y, int w, int h)
{
    SDL_Point points[5]; 

    points[0].x = x;
    points[0].y = y;

    points[1].x = x+w;
    points[1].y = y;

    points[2].x = x+w;
    points[2].y = y+h;

    points[3].x = x;
    points[3].y = y+h;

    points[4].x = x;
    points[4].y = y;

    SDL_RenderDrawLines(screen,points,5);
}

void draw_ground()
{
    SDL_Rect rect;
    bool out = false;
    scene_coordinates(SCENE_MIN_X,0.0,&rect.x,&rect.y,&out);
    scene_coordinates(SCENE_MAX_X,SCENE_MIN_Z,&rect.w,&rect.h,&out);
    rect.h -= rect.y;
    rect.w = scene_width;
            
    SDL_SetRenderDrawColor(screen, GROUND_R, GROUND_G, GROUND_B, GROUND_A);
    SDL_RenderFillRect(screen, &rect);
}

void draw_initial_state()
{
    draw_initial_position();
    draw_initial_velocity();
}

void draw_initial_position()
{
    SDL_SetRenderDrawColor(screen, 0x00, 0xFF, 0x00, 0xFF);
    draw_square(INITIAL_STATE[PX],INITIAL_STATE[PZ],SQUARE_WIDTH);
}

void draw_square(double px, double pz, int w)
{
    SDL_Rect rect;
    bool out = false;    
    
    scene_coordinates(px,pz,&rect.x,&rect.y,&out);
    
    if (out) return;

    rect.x -= (w-1)/2;
    rect.y -= (w-1)/2;
    rect.w = w;
    rect.h = w;

    SDL_RenderFillRect(screen, &rect);
}

void draw_initial_velocity()
{
    SDL_SetRenderDrawColor(screen, 0x00, 0xFF, 0x00, 0xFF);
    draw_arrow(INITIAL_STATE[PX],INITIAL_STATE[PZ],
                INITIAL_STATE[VX],INITIAL_STATE[VZ],
                VELOCITY_DRAW_FACTOR);
}

void draw_arrow(double x, double y, double u, double v, double k)
{
    SDL_Point pts[2];
    bool out;

    scene_coordinates(x,y,&pts[0].x,&pts[0].y,&out);

    if (out) return; // dont draw arrow if base outside of scene

    scene_coordinates(x+k*u,y+k*v,&pts[1].x,&pts[1].y,&out);

    SDL_RenderDrawLines(screen,pts,2);
}

void draw_objective()
{    
    SDL_SetRenderDrawColor(screen, 0xFF, 0x00, 0x00, 0xFF);
    draw_square(0.0,0.0,SQUARE_WIDTH);
}

void draw_trajectory()
{
    if (state_list == NULL) return;

    SDL_SetRenderDrawColor(screen, 0x00, 0x00, 0x00, 0xFF);

    draw_state_list(state_list);
}

void draw_state_list(struct state_list_t * list)
{
    if (list == NULL) return;

    struct state_list_t *local_list = list;

    bool out = false;
    int x = 0, y = 0;

    while (local_list != NULL)
    {
        if (local_list->state != NULL)
        {
            scene_coordinates(local_list->state[PX],local_list->state[PZ],&x,&y,&out);

            if (!out) SDL_RenderDrawPoint(screen,x,y);
        }      

        local_list = local_list->next;
    }
}

void draw_predicted_trajectory()
{
    if (!PREDICT) return;
    
    struct state_list_t * predicted = predict(state_list);
    if (predicted == NULL) return;

    SDL_SetRenderDrawColor(screen, 0x77, 0x88, 0x99, 0xFF);

    draw_state_list(predicted);

    free_state_list(predicted);
}

void draw_current_state()
{
    draw_current_position();
    draw_current_velocity();
    draw_current_thrust();
}

void draw_current_position()
{
    if (state_list == NULL) return;
    if (state_list->state == NULL) return;
            
    SDL_SetRenderDrawColor(screen, 0x00, 0x00, 0x00, 0xFF);
    draw_square(state_list->state[PX],state_list->state[PZ],SQUARE_WIDTH);
}

void draw_current_velocity()
{
    if (state_list == NULL) return;
    if (state_list->state == NULL) return;
    
    SDL_SetRenderDrawColor(screen, 0x00, 0x00, 0xFF, 0xFF);
    draw_arrow(state_list->state[PX],state_list->state[PZ],
                state_list->state[VX],state_list->state[VZ],
                VELOCITY_DRAW_FACTOR);
}

void draw_current_thrust()
{
    if (is_dry) return;
    
    SDL_SetRenderDrawColor(screen, 0xFF, 0x00, 0x00, 0xFF);
    draw_arrow(state_list->state[PX],state_list->state[PZ],
                current_thrust_x,current_thrust_z,
                THRUST_DRAW_FACTOR);
}

void draw_gravity()
{
    
}

void draw_mass()
{
    if (is_dry) 
    {
        draw_mass_frame();
        return;
    }

    double mass = state_list->state[M]-DRY_MASS;
    double half = (WET_MASS-DRY_MASS)/2.0;

    if (mass >= half)
        SDL_SetRenderDrawColor(screen, 0x00, 0xFF, 0x00, 0xFF);
    
    if (mass < half && mass >= half/2.0)
        SDL_SetRenderDrawColor(screen, 0xFF, 0x8C, 0x00, 0xFF);

    if (mass < half/2.0)
        SDL_SetRenderDrawColor(screen, 0xFF, 0x00, 0x00, 0xFF);

    double ratio = mass/(WET_MASS-DRY_MASS);

    SDL_Rect rect;    
    rect.w = scene_height/12;
    rect.h = ratio*scene_height/3;
    rect.x = scene_x+WINDOW_MARGIN;
    rect.y = scene_y+WINDOW_MARGIN + scene_height/3 - rect.h;

    SDL_RenderFillRect(screen,&rect);

    draw_mass_frame();
}

void draw_mass_frame()
{
    SDL_SetRenderDrawColor(screen, 0x00, 0x00, 0x00, 0xFF);
    draw_frame(scene_x+WINDOW_MARGIN, scene_y+WINDOW_MARGIN, scene_height/12, scene_height/3);
}

void draw_all()
{
    draw_scene();
    draw_mass();
}