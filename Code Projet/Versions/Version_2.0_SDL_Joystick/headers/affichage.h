

#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__


extern int shoulder;


//Valeurs d'affichage
int renderScaling = 6; //multiplie les coordonnées du code par 5 pour l'affichage



int HEIGHT = 900;
int WIDTH = 900;

int X_BASE = 150; //Epaule
int Z_BASE = HEIGHT - (HEIGHT / 6); //affiche en partant du haut

int X_SHOULDER = X_BASE;
int Z_SHOULDER = Z_BASE - renderScaling * shoulder;

float ETA_INIT = 0;



//coordonnées pour la position en cm
//Correspond à la position de Reset et à la position Initiale

int X_INIT = 10;
int Y_INIT = 0;
int Z_INIT = 0;



//Fonctions

void renderLine(float x1, float y1, float x2, float y2);

void renderThickLine(float x1, float y1, float x2, float y2);

void renderPivot(int x, int y);



#endif