#include <cstdio>
#include <stdlib.h> //contient sleep
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <SDL2/SDL.h>

#include "../headers/fonctions.h"
//#include "../headers/affichage.h"
//#include "../headers/variables.h"



void reset(bool& IsReset, float& x, float& y, float& z, float& eta) {
    if (IsReset) {
        std::cout << "Entering Reset.\n";

        if (x < X_INIT - MAX_INPUT || x > X_INIT + MAX_INPUT) {
            Input_Processing(std::copysign(MAX_INPUT/2, -(x - X_INIT)), 0, 0, x, y, z, eta);//Copysign(x, y) returns x*(sign of y)
        }
        else if (z < Z_INIT - MAX_INPUT || z > Z_INIT + MAX_INPUT) {
            Input_Processing(0, 0, std::copysign(MAX_INPUT/2, -(z - Z_INIT)), x, y, z, eta);
        }
        else if (y < Y_INIT - MAX_INPUT || y > Y_INIT + MAX_INPUT) {
            Input_Processing(0, std::copysign(MAX_ROTATION_INPUT/2, -(y - Y_INIT)), 0, x, y, z, eta);
        }


        else {
            IsReset = false;
            std::cout << "Reset done.\n";
        }
    }
} //r�initialise axe par axe la position du robot. Potentiellement directement par angle plut�t que par position (seulement avec une commande en angle). 
//Un fois la r�initialisation effectu�e, rend False le bool de r�initialisation pour sortir de la boucle.


void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float& eta)  
{

    //mise � jour des coordonn�es
    float x = X + input_x;
    float y = Y + input_y;
    float z = Z + input_z;



    float length = sqrt(x * x + z * z); //calcul de la taille totale en cm

    if (length > arm + forearm - MAX_INPUT || x < 0) { //v�rification qu'il n'est pas possible d'aller plus loin. 
        //Probl�me actuel : bloque tous les mouvements, pas juste celui qui pose probl�me. Un peu rigide. 
        std::cout << "\nMax position reached, can't go any further\n" << std::endl;

    }
    else {



        float alpha = exact_answer(x, z);

        //std::cout << "\nFinal alpha is " << alpha << " in radians\n";


        //Definitions
        float beta;
        float gamma;



        double inside_c = (x - arm * cos(alpha)) / forearm; // c for cos
        double inside_s = (z - arm * sin(alpha)) / forearm;



        if (inside_c > sqrt(2) / 2 || z < -30) { //empirique : lorsque inside_c = sqrt(2)/2, inside_s aussi et � ces valeurs, arcsin et arccos sont �gaaux : on peut passer d el'un a� l'autre
            //correspond � z partiellement n�gatif
            //Probl�me : pour z sufisamment bas, inside_c repasse en dessous de acine2/2, donc on va poser un palier arbitraire dans lequel on utilisera toujours arcsin.

            if (inside_s < 0.9999 || inside_s > -0.9999) {

                gamma = asin(inside_s);
                beta = gamma - alpha;
            }
            else {
                beta = -alpha;
            }

        }

        //pas de situations ou le contenu de arccos est inf�rieur � -1 (ou m�me � 0) car pas de x n�gatif

        else {

            gamma = acos(inside_c);

            beta = gamma - alpha;
        }

        // Attention au domaine de d�finition de Acos esp�ce de b�ta !!!
        //Beta est n�cesaire aux moteurs



        //faire quelque chose avec les angles, typiquement les donner commme arguments en vitesse dans un moteur.
        //Dans cette fonction ou une autre ?

        //Pas besoin du bool Click : le fait de savoir si on va vers le haut ou l'avant sera d�termin� avant au moment de mettre les inputs en arguments. 


        //Update motors selon x, y et z



        float r_X = (arm * cos(alpha) + forearm * cos(gamma)) - X;
        float r_Z = (arm * sin(alpha) + forearm * sin(gamma)) - Z; //d�placements
        float r_Y = input_y;
        //gamma est l'angle du second axe par rapport � l'horizontal, pas beta. Il faut donc prendre gamma pour calculer les cosinus. 

        //std::cout << "Translation on X due to first motor is  " << arm * cos(alpha) 
         //   << "\nTranslation on X due to second motor is  " << forearm * cos(gamma);

        //std::cout << "\nTrue input value changes are " << r_X << " for the horizontal plan,  " << input_y << " for the rotationnal plan and " << r_Z <<" for the vertical plan\n";

        //Mise � jour des coordonn�es : faire touner en fonction de l'angle. Comme la commande est en vitesse, il faudra faire tourner le moteur
        //� une certaine vitesse pendant une frame. La vitesse d�pendra de la distance finale � parcourir, mais �galement de la position actuelle.

        //attention ! la vitesse de rotation ne pourra pas �tre constante pour toute extension. Lorsque le poignet
        //est tr�s pr�s, la moindre rotation aura une grande influence. Il faudra dont en quelque sorte multiplier par une fonction 
        //croissante de la distance. Tests � r�aliser. 


        //Coordonn�es du poignet
        X += r_X;
        Z += r_Z;
        Y += r_Y; //ce qui arrive dans les faits : il faut donc corriger input_y pour r�duire l'influence de length.


        //variation de eta = etaprec - etanow soit etaprec + gamma pour pouvoir mettre � jour le moteur.

        float eta_var = eta + gamma;
        //faire tourner le moteur comme il se doit avec Eta_var

        eta = -gamma; //mise � jour de eta

        //tests
        x = X + wrist * cos(alpha + beta + eta);
        y = Y;
        z = Z + wrist * sin(alpha + beta + eta);

        //positions d'affichage
        int x_elbow = (X_SHOULDER + renderScaling * arm * cos(alpha));
        int z_elbow = (Z_SHOULDER - renderScaling * arm * sin(alpha));

        int x_wrist = (X_SHOULDER + renderScaling * X);
        int z_wrist = (Z_SHOULDER - renderScaling * Z);

        int x_end = x_wrist + renderScaling * wrist * cos(alpha + beta + eta);
        int z_end = z_wrist - renderScaling * wrist * sin(alpha + beta + eta); //signe - car affichage � l'envers

        //affichage
        renderThickLine(X_SHOULDER, Z_SHOULDER, x_elbow, z_elbow);
        renderThickLine(x_elbow, z_elbow, x_wrist, z_wrist);
        renderThickLine(x_wrist, z_wrist, x_end, z_end);

        renderPivot(x_elbow, z_elbow);
        renderPivot(x_wrist, z_wrist);
        renderPivot(X_SHOULDER, Z_SHOULDER);

        /*std::cout << "\nCoordonnees du poignet\n" << std::setprecision(1) << std::fixed << std::setw(4) << x << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << y << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << z << std::endl;*/



    }
}//Recoit les inputs, traduit les inputs en modification de position et calcule les commandes n�cessaires pour effecuer ce changement. 
//re�oit la position actuelle, les inputs, et la valeur pr�c�dente de eta servant � piloter le poignet.


float exact_answer(float x, float z) {

    float X = (arm * arm - forearm * forearm + x * x + z * z) / (2 * arm);
    float length = sqrt(x * x + z * z);

    float phi = acos(z / length);

    float blub = asin(X / length);

    float alpha = blub - phi; //La solution qui marche (l'autre ne parche pas pour z n�gatif


    return alpha;

}; //precision, x et y et alpha_pr�c�dent


void delay(int temps) { //en ticks
    int t1 = clock();
    int t2 = clock();
    int delta = t2 - t1;

    while (delta < temps) {
        t2 = clock();
        delta = t2 - t1;
    }
}//pour d�layer les frames


bool init(){
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
        success = false;
    }

    //Check for joysticks
    if (SDL_NumJoysticks() < 1)
    {
        printf("Warning: No joysticks connected!\n");
        success = false;
    }

    else
    {
        //Load joystick
        gGameController = SDL_JoystickOpen(0);
        if (gGameController == NULL)
        {
            printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
    }
    std::cout << "\nNumber of joysticks is : " << SDL_NumJoysticks() << std::endl;

    std::cout << "\nNumber of buttons is : " << SDL_JoystickNumButtons(gGameController) << std::endl;

    return success;
}//initialise l'�cran et le joystick


bool boucleFermee(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float& eta, float& corr_x, float& corr_y, float& corr_z){
// corr_x et autres sont les informations reçues par les capteurs de l'étape précédente. On les passe en référence pour les enregistrer pour l'étape suivante.
// On commencela boucle avec les infos de l'étape précédente, puis on récupère les nouvelles infos pour l'étape suivante. 







}//reçoit les positions actuelles, les positions voulues ainsi que les corrections de la dernière fois





/*

Plan pour la boucle fermée : 
Lors d'un mouvement, la machine va recevoir un input. A partir de cet input, elle va calculer la future position, puis déplacer les moteurs vers cette position. 
Des capteurs vont ensuite mesurer cette nouvelle position.

Que faire de cette information ?
Dans un premier lieu, corriger la position actuelle pour éviter des erreurs de calcul. Etant donné que le calcul de la position future dépend principalement de la position actuelle, 
Il est impératif de faire en sorte que la position actuelle n'est pas compromise.
Maintenant, que faire ? 
Si la commande est en retard de manière consistente, Il faut sans doute compenser le manque d'une manière ou d'une autre. 
=> on pourrait par exemple garder la différence entre le dernier input et les capteurs, et ajouter cela au prochain input pour compenser ?

*/