

#ifndef __VARIABLES_H__
#define __VARIABLES_H__

const int JOYSTICK_DEAD_ZONE = 9000;
float MAX_PRESS_TIME = 0.7; //en secondes


float fps = 60;

int Max_speed = 60;//En cm par seconde

float MAX_INPUT = Max_speed / fps;//d�lacement max par frame
float MAX_ROTATION_INPUT = 1.2 / fps; //radian.s-1 de rotation de l'axe vertical





//Normalized direction

//Taille des membres en mm
int arm = 35;
int forearm = 35;
int shoulder = 35;

int wrist = 12;

float pi = 3.14159;





//Variables

extern SDL_Renderer* sdlRenderer;
SDL_Joystick* gGameController = NULL; //pour le joystick

SDL_Event e;//Event handler



#endif