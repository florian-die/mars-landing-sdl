#include "game.h"

#include "dynamics.h"
#include "sdl_utils.h"
#include "draw.h"

#include <SDL2/SDL.h>
#include <stddef.h>

// Game state
bool GAME_PAUSED = true; // Pause game if set to true
bool GAME_OVER = false;
bool QUIT = false; // Exit program if set to true

bool PREDICT = true;

double joy_thrust_x = 0.0;
double joy_thrust_z = 0.0;
double joy_thrust_n = 0.0;

int init_game()
{
    print_start_ascii();
    print_start_help();
    
    // Initialize SDL
    if (!init_sdl())
    {
        printf("Failed to initialize SDL\n");
        return -1;
    }
    init_scene();

    // Initial dynamical system
    if (init_state_list() == NULL)
    {
        printf("Failed to initialize lander\n");
        return -1;
    }
    init_dynamics();    

    // Start the timer
    init_timer();

    render_screen();

    draw_all();

    render_pause();

    SDL_RenderPresent(screen);

    return 0;
}

void print_start_ascii()
{
    printf("====================================\n");
    printf("=========== MARS LANDING ===========\n");
    printf("====================================\n");
    printf("=            __________            =\n");
    printf("=           |          |           =\n");
    printf("=           |          |           =\n");
    printf("=           |__________|           =\n");
    printf("=          /|          |\\          =\n");
    printf("=         /_|          |_\\         =\n");
    printf("=          U            U          =\n");
    printf("=          U            U          =\n");
    printf("=                                  =\n");
    printf("====================================\n");
}

void print_start_help()
{
    printf("====================================\n");
    printf("============= Controls =============\n");
    printf("====================================\n");
    printf("= Start / Pause    : Start Button  =\n");
    printf("= Reset            : Back Button   =\n");
    printf("= Thrust Direction : Left Stick    =\n");
    printf("= Thrust Magnitude : Right Trigger =\n");
    printf("= Show Prediction  : Y Button      =\n");
    printf("====================================\n");
    printf("============ TANGO DELTA ===========\n");
    printf("====================================\n");
}

void loop_game()
{
    // Main loop
    while (!QUIT)
    {
        // Event loop
        while (SDL_PollEvent(&event))
        {
            handle_events();
        }

        compute_thrust();

        // Rendering loop
        if(!GAME_PAUSED && !GAME_OVER)
        {
            forward();

            print_current_state();            
        }  

        render_screen();

        draw_all();

        if (GAME_PAUSED) render_pause();

        SDL_RenderPresent(screen);             
    }    
}

void handle_events()
{
    if(event.type == SDL_QUIT)
    {
        QUIT = true;
    }

    handle_joy_axis();

    handle_joy_buttons();
}

void handle_joy_axis()
{
    // joy axis disabled once grounded
    if(is_grounded) return;
    
    if(event.type == SDL_JOYAXISMOTION)
    {
        // left stick / left-right = 0
        if(event.jaxis.axis == 0)
        {
            joy_thrust_x = (double) (event.jaxis.value) / (double) (MAX_JOYSTICK_AXIS_VALUE);
            saturate(&joy_thrust_x,-1.0,1.0);
            normalize(&joy_thrust_x,&joy_thrust_z);
            // printf("Unit Joy X = %f, Z = %f\n",joy_thrust_x,joy_thrust_z);
        }

        // left stick / top-down == 1
        if(event.jaxis.axis == 1)
        {
            joy_thrust_z = - (double) (event.jaxis.value) / (double) (MAX_JOYSTICK_AXIS_VALUE);
            saturate(&joy_thrust_z,-1.0,1.0);
            normalize(&joy_thrust_x,&joy_thrust_z);
            // printf("Unit Joy X = %f, Z = %f\n",joy_thrust_x,joy_thrust_z);
        }

        // right trigger == 5
        if(event.jaxis.axis == 5)
        {
            joy_thrust_n = (double) (event.jaxis.value+MAX_JOYSTICK_AXIS_VALUE);
            joy_thrust_n /= (double) (2*MAX_JOYSTICK_AXIS_VALUE);
            saturate(&joy_thrust_n,0.0,1.0);
            // printf("Unit Joy N = %f\n",joy_thrust_n);
        }
        
        // printf("Event JoyAxisMotion {axis: %i, value: %i}\n",
        //     event.jaxis.axis, event.jaxis.value);
    }
}

void saturate(double *x, double min, double max)
{
    if(*x > max) *x = max;
    if(*x < min) *x = min;
}

void normalize(double *x, double *z)
{
    double norm = sqrt((*x)*(*x)+(*z)*(*z));

    if (norm == 0.0) return;
    
    *x /= norm;
    *z /= norm;
}

void handle_joy_buttons()
{
    if(event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
    {
        // Pause
        if(event.jbutton.button == 7 && event.jbutton.state == SDL_PRESSED)
        {
            GAME_PAUSED = !GAME_PAUSED;

            if(!GAME_PAUSED)
            {
                timer.previous_tick = SDL_GetTicks();
            }
            
        }

        // Reset
        if(event.jbutton.button == 6 && event.jbutton.state == SDL_PRESSED)
        {
            // printf("Reset\n");
            
            init_state_list();
            
            init_timer();

            init_dynamics();

            GAME_OVER = false;

            GAME_PAUSED = true;
        }

        // predicted / 3 = Xbox Y
        if(event.jbutton.button == 3 && event.jbutton.state == SDL_PRESSED)
        {
            PREDICT = !PREDICT;
        }

        // printf("Event JoyButton {button: %i, state: %i}\n",
        //     event.jbutton.button, event.jbutton.state);
    }
}

void render_screen()
{
    // Clear screen with white
    SDL_SetRenderDrawColor(screen, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(screen);

    // Update screen
    // SDL_SetRenderDrawBlendMode(screen, SDL_BLENDMODE_BLEND);
    // SDL_RenderPresent(screen);

    // Free too old states
    // shorten_state_list(state_list,timer.current_tick/1000.0-PERSISTENCE_DURATION);
}

void render_pause()
{
    SDL_SetRenderDrawColor(screen, 0x00, 0x00, 0x00, 0xAF);
    
    int width = 30;
    int height = 100;
    int space = 20;

    SDL_Rect rect;
    rect.w = width;
    rect.h = height;
    rect.x = SCREEN_WIDTH/2 + space/2;
    rect.y = SCREEN_HEIGHT/2 - height/2;
            
    SDL_RenderFillRect(screen, &rect);

    rect.x = rect.x - space - width;

    SDL_RenderFillRect(screen, &rect);
}

void quit_game()
{    
    quit_sdl();
}