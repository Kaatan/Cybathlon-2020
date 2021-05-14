#include <cstdio>
#include <stdlib.h> //contient sleep
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <SDL2/SDL.h>
#include <time.h>

#include "../headers/affichage.h"
#include "../headers/variables.h"
#include "../headers/fonctions.h"
#include "serial.h"


// for serial
char serialData[256] = "";			// don't forget to pre-allocate memory
            //printf("%s\n",incomingData);
            int dataLength = 255;
            int readResult = 0;

int t1;
int t2;
float delta; // For chrono


int press_time;
int release_time;
int press_duration = 0;
int verif; //For processing
bool is_pressed = false;

bool is_stop = false; //for main while

int clickmode = 0;
// For button

int is_reset = 0; //true en avance pour r�initialiser d�s le d�marrage et ainsi bien repartir de z�ro

float x_input;
float y_input;


Serial* serial = new Serial("COM0");





int main(int argc, char** argv) {


    


    

    float x = X_INIT;
    float y = Y_INIT;
    float z = Z_INIT; //les trois positions cardinales qui seront utilis�es pour le robot, en cm
    //Pour les tests, x, y et z iront de 0 � 999, � la premi�re d�cimale

    float eta = ETA_INIT;






    init(); //Initialise SDL et le Joystick
    
    //SDL_Surface* g_screenSurface;
    // SDL_Window* window;


    // window = SDL_CreateWindow("Affichage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    //     WIDTH, HEIGHT, SDL_WINDOW_SHOWN); //Cr�ation de la fen�tre vid�o

    // if (window == NULL) {
    //     printf("Impossible de cr�er l'affichage de dimensions %dx%d : %s\n", WIDTH, HEIGHT, SDL_GetError());
    //     SDL_Quit();
    //     return -1;  
    // }

    // sdlRenderer = SDL_CreateRenderer(window, -1, 0); //Cr�ation d'un truc pour render

    // if (sdlRenderer == NULL) {
    //     printf("Impossible de cr�er le renderer de dimensions %dx%d : %s\n", WIDTH, HEIGHT, SDL_GetError());
    //     SDL_Quit();
    //     return -1;
    // }

    //SDL affichage

    printf("Veuillez utiliser une manette pour utiliser ce programme.\nLe joystick gauche permet de piloter le bras\n");
    printf("L'axe haut/bas du joystick permet de deplacer le bras vers l'avant ou l'arriere\nPour deplacer le bras sur l'axe vertical, appuyer une fois sur le bouton du bas de la partie droite de la manette.\n");
    printf("Le bras sera a lors en configuration deplacement vertical.\n");
    printf("L'affichage actuel ne permet pas d'afficher le d�placement lateral, mais appuyer le joystick sur les directions laterales deplace le bras.\n");
    printf("Pour r�initialiser la position du bras dans sa position initiale, appuyer une fois sur le bouton droit du c�t� droit de la manette.\n");
    printf("Pour sortir de l'application, appuyer longuement sur le bouton du bas de la manette (le m�me que pour changer de mode)");
    printf("Le bras se r�initialise tout seul lors du lancement du programme.\n");


    while (!is_stop) {
        t1 = clock(); //d�but du chrono
        //std::cout << "\n" << t1 << std::endl;

        // if (is_reset) { //si le syst�me est en reset, on attend la fin du reset
        //     std::cout << "Resetting ...\n";
        //     t1 = clock();
            
            
        //     SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);//sdlrenderer d�fini depuis afichage.h
        //     if (SDL_RenderClear(sdlRenderer) != 0) { //On peint tout
        //         printf("Erreur de coloration\n");

        //     }
        //     reset(is_reset, x, y, z, eta);

            

        //     // renderThickLine(X_BASE, Z_BASE, X_SHOULDER, Z_SHOULDER);
        //     // renderPivot(X_BASE, Z_BASE);
        //     // renderPivot(X_SHOULDER, Z_SHOULDER); //Affichage des membres de base (bras et �paule)
        //     // SDL_RenderPresent(sdlRenderer);//mise � jour de l'�cran

        //     t2 = clock();

        //     if ((float)delta / CLOCKS_PER_SEC < (float)(1 / fps)) {
        //         //std::cout<<"\nblub delay\n";
        //         delay(CLOCKS_PER_SEC / fps - delta);

        //     } //pour avoir du 60 fps


        // }
        else {//sinon


            while (SDL_PollEvent(&e) != 0)
            {
                
                //User requests quit
                if (e.type == SDL_QUIT)
                {
                    is_stop = true;
                }
                


                else if (e.type == SDL_JOYAXISMOTION) //s'il y a un joystick utilis�
                {
                    //Motion on controller 0
                    if (e.jaxis.which == 0)
                    {
                        //X axis motion
                        if (e.jaxis.axis == 1)
                        {
                            //Left of dead zone
                            if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                            {
                                x_input = - e.jaxis.value;
                            }
                            //Right of dead zone
                            else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
                            {
                                x_input = - e.jaxis.value;

                                
                            }
                            else
                            {
                                x_input = 0;
                                
                            }
                        }
                        //Y axis motion
                        else if (e.jaxis.axis == 0)
                        {
                            //Below of dead zone
                            if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
                            {
                                y_input = e.jaxis.value;
                                //std::cout << "\n   " << y_input << "\n";
                            }
                            //Above of dead zone
                            else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
                            {
                                y_input = e.jaxis.value;
                                //std::cout << "\n   " << y_input << "\n";
                            }
                            else
                            {
                                y_input = 0;
                                
                            }
                        }
                    }
                    // x_input = x_input / 32768 * MAX_INPUT;
                    // y_input = y_input / 32768 * MAX_INPUT;
                }


                else if (e.type == SDL_JOYBUTTONDOWN) {

                    if ((int)e.jbutton.button == 1) {
                        std::cout << "Resetting.\n"; //Bouton de droite du controller
                        is_reset = true;
                    }

                    else { //N'importe quel autre bouton
                        //std::cout << "Pressed the button " << (int)e.jbutton.button << std::endl;
                        press_time = clock();
                        is_pressed = true; //la premi�re pression a �t� r�alis�e, si elle est maintenue, on peut �teindre
                    }
                }



                else if (e.type == SDL_JOYBUTTONUP) {
                    if ((int)e.jbutton.button != 1) { //1 correspond au reset, un message a d�j� �t� dit. De plus le reset ne compte pas comme un input normal. 

                        //std::cout << "Released !\n";

                        if (!clickmode) {
                            clickmode = 1;
                        }
                        else {
                            clickmode = 0;
                        }
                        is_pressed = false;
                    }
                }



            }



            //traitement des valeurs re�ues en terme de puissance par rapport au maxinput

            
            

            // SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);//sdlrenderer d�fini depuis afichage.h

            // if (SDL_RenderClear(sdlRenderer) != 0) { //On peint tout
            //     printf("Erreur de coloration\n");

            // }

            // renderThickLine(X_BASE, Z_BASE, X_SHOULDER, Z_SHOULDER);
            // renderPivot(X_BASE, Z_BASE);
            // renderPivot(X_SHOULDER, Z_SHOULDER); //Affichage des membres de base (bras et �paule)

            
            
            

            // if (clickmode) {
            //     Input_Processing(0, y_input, x_input, x, y, z, eta);
            // }
            // else{
            //     Input_Processing(x_input, y_input, 0, x, y, z, eta);
            //     //std::cout << x_input << " " << y_input << std::endl;
            // }

            // if ((((float)(t2 - press_time) / CLOCKS_PER_SEC) > MAX_PRESS_TIME) && is_pressed) {

            //     is_stop = true;
            //     std::cout << "End of Programm\n";

            // } //Maintenir le bouton sort du programme. 

            // SDL_RenderPresent(sdlRenderer);//mise � jour de l'�cran

            t2 = clock();
            //std::cout << t2 << std::endl;
            delta = t2 - t1;
            //std::cout << "\nDelta (Ticks) " << delta << std::endl;
            //std::cout << "Delta (secondes) " << delta/CLOCKS_PER_SEC << std::endl;
            //std::cout << "Limite (secondes) " << (float)(1/fps) << std::endl;



            if ((float)delta/CLOCKS_PER_SEC < (float)(1 / fps) ) {
                //std::cout<<"\nblub delay\n";
                delay(CLOCKS_PER_SEC / fps - delta);
                
            } //pour avoir du 60 fps


            sprintf(serialData, "Joystick X = %f, Joystick Y = %f, Button is %d, Reset is %d, joystick Max is : %f", input_x, input_y, clickmode, is reset, 32768);

            
            
            serial.WriteData("Joystick X = %f, Joystick Y = %f, Button is %d, Reset is %d, joystick Max is : %f\n", 100);


            std::cout <<"Sent" << serialData<<std::endl;

            if (clickmode = 1){
                clickmode = 0;
            }

            

            

            // std::cout << "\r" << std::setprecision(1) << std::fixed << std::setw(4) << x << " "
            //     << std::setprecision(1) << std::fixed << std::setw(4) << y << " "
            //     << std::setprecision(1) << std::fixed << std::setw(4) << z; //Affichage des coordonn�es

        }
    }




}