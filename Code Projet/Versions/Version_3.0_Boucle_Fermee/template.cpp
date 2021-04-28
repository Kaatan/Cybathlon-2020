#include <cstdio>
#include <stdlib.h> //contient sleep
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <SDL2/SDL.h>
#include <time.h>


//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 9000;
float MAX_PRESS_TIME = 0.7; //en secondes


//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;



void delay(int temps) {
    int t1 = clock();
    int t2 = clock();
    int delta = t2 - t1;

    while (delta < temps) {
        t2 = clock();
        delta = t2 - t1;
    }
}//pour délayer les frames


bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }

    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        printf("Warning: Linear texture filtering not enabled!");
    }

    //Check for joysticks
    if (SDL_NumJoysticks() < 1)
    {
        printf("Warning: No joysticks connected!\n");
    }
    
    else
    {
        //Load joystick
        gGameController = SDL_JoystickOpen(0);
        if (gGameController == NULL)
        {
            printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
        }
    }
    std::cout << "Number of joysticks is : " << SDL_NumJoysticks()<<std::endl;

    std::cout << "Number of buttons is : " << SDL_JoystickNumButtons(gGameController)<<std::endl;
}


//Main loop flag
bool quit = false;

//Event handler
SDL_Event e;

//Normalized direction
int xDir = 0;
int yDir = 0;

int t1;
int t2;
int delta;


int press_time;
int release_time;
int press_duration = 0;
int verif;

int main(int argc, char** argv){

    bool is_stop = false;

    bool clickmode = false;

    bool is_pressed = false;

    init();
    //Handle events on queue
    while (! is_stop) {
        t1 = clock(); //début du chrono
        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_JOYAXISMOTION) //s'il y a un joystick utilisé
            {
                //Motion on controller 0
                if (e.jaxis.which == 0)
                {
                    //X axis motion
                    if (e.jaxis.axis == 0)
                    {
                        //Left of dead zone
                        if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                        {
                            xDir = e.jaxis.value;
                        }
                        //Right of dead zone
                        else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
                        {
                            xDir = e.jaxis.value;
                        }
                        else
                        {
                            xDir = 0;
                        }
                    }
                    //Y axis motion
                    else if (e.jaxis.axis == 1)
                    {
                        //Below of dead zone
                        if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                        {
                            yDir = e.jaxis.value;;
                        }
                        //Above of dead zone
                        else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
                        {
                            yDir = e.jaxis.value;;
                        }
                        else
                        {
                            yDir = 0;
                        }
                    }
                }
            }


            else if (e.type == SDL_JOYBUTTONDOWN) {

                if ((int)e.jbutton.button == 1) {
                    std::cout << "Resetting. Button\n";
                }

                else {
                    std::cout << "Pressed the button " << (int)e.jbutton.button << std::endl;
                    press_time = clock();
                    is_pressed = true; //la première pression a été réalisée, si elle est maintenue, on peut éteindre
                }
            }

           

            else if (e.type == SDL_JOYBUTTONUP) {
                
                std::cout << "Released ! Loul\n";
                
                clickmode = true;
                is_pressed = false;
                
            }

            

            else if (e.type == SDL_KEYUP) {
                std::cout << "Key pressed.\n";
                if (e.key.keysym.sym == SDLK_r) {
                    std::cout << "Resetting.\n";
                }
            }
            else
            {
                std::cout << "Blub\n";
            }
            
        }

        


        std::cout << "\r" << std::fixed << std::setw(4) << xDir << "   " << std::fixed << std::setw(4) << yDir;


        t2 = clock();
        delta = t2 - t1;
        if (delta < 1 / 60) {
            delay(1 / 60 - delta);
        } //pour avoir du 60 fps


        

        if ((((float)(t2 - press_time) / CLOCKS_PER_SEC) > MAX_PRESS_TIME)&& is_pressed) {
            
            is_stop = true;
            std::cout << "End of Programm\n";

        } //Maintenir le bouton sort du programme. 



    }

};