

#ifndef __FONCTIONS_H__
#define __FONCTIONS_H__

extern int X_INIT;
extern int Y_INIT;
extern int Z_INIT;

extern float MAX_INPUT;
extern float MAX_ROTATION_INPUT;

extern int arm;
extern int forearm;
extern int wrist;
extern int shoulder;

extern SDL_Joystick* gGameController;

extern int X_BASE;
extern int Z_BASE;

extern int HEIGHT;
extern int WIDTH;

extern int X_SHOULDER;
extern int Z_SHOULDER;

extern int renderScaling;

extern void renderLine(float x1, float y1, float x2, float y2);

extern void renderThickLine(float x1, float y1, float x2, float y2);

extern void renderPivot(int x, int y);




//Fonctions
void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float& eta);

void reset(bool& IsReset, float& x, float& y, float& z, float& eta);

float exact_answer(float x, float z);

void delay(int temps);

bool init();

#endif