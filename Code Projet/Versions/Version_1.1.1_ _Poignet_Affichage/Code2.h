#pragma once

#ifndef __CODE2_H__
#define _CODE2_H__


//FONCTIONS
void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float& eta);

void reset(bool& IsReset, float& x, float& y, float& z, float& eta);

int Test_Input_Recognition(const char x);

void Test_Input_Translator(int command, bool click, float& x, float& y, float& z, float& eta);

float exact_answer(float x, float z);

//Variables

extern SDL_Renderer* sdlRenderer;


float frame = 10; //dur�e de base des inputs en millisecondes. Le cycle a lieu chaque frame, mais la dur�e des commandes une fois effectu�es sera un peu plus longue pour assurer la fluidit�, bien qu'elle devra pouvoir �tre r��crite.  
//A titre de comparaison, une frame dans un JV dure 16.6 ms � 60fps. 
//La valeur de 50 est juste l� pour les tests et il serait raisonnable de descendre � 16 ou moins. 

float MAX_INPUT = 60 * frame / 1000; //Unit� de d�placement en cm par input. 60 cm par seconde

float MAX_ROTATION = 1.2; //Angle.s-1 de rotation de l'axe vertical

//reconnaissance des inputs
int avant = 0;
int arriere = 1;
int gauche = 2;
int droite = 3;
int clic = 4;
int end = 5;
int unknown = 6;
int ResetDigit = 7;

//Taille des membres en mm
int arm = 35;
int forearm = 35;
int shoulder = 35;

int wrist = 12;

float pi = 3.14159;


//Coordonn�es pour l'affichage
int renderScaling = 5; //multiplie les coordonn�es du code par 5 pour l'affichage



int HEIGHT = 900;
int WIDTH = 900;

int X_BASE = 300; //Epaule
int Z_BASE = HEIGHT - (HEIGHT / 3); //affiche en partant du haut

int X_SHOULDER = X_BASE;
int Z_SHOULDER = Z_BASE - renderScaling * shoulder;

float ETA_INIT = 0;



//coordonn�es pour la position en cm
//Correspond � la position de Reset et � la position Initiale

int X_INIT = 10;
int Y_INIT = 0;
int Z_INIT = 0;
#endif