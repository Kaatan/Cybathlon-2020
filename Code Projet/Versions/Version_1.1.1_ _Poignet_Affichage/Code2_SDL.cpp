//Code de test
#include <cstdio>
#include <stdlib.h> //contient sleep
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <SDL2/SDL.h>


#include <conio.h> //n�cessaire pour recevoir les inputs de la console sans validation

#include "Code2.h"
#include "affichage.h"


//###################################################################################
// ###########   CE CODE SERVIRA DE TEMPLATE POUR LA SUITE   ########################
// IL CONTIENT LE PRINCIPE DE BASE POUR L'ACQUISITION DES INPUTS VIA SLEEP ET CONIO #
//###################################################################################




void reset(bool& IsReset, float& x, float& y, float& z,  float& eta) {
    if (IsReset) {

        if (x < X_INIT - MAX_INPUT || x > X_INIT + MAX_INPUT) {
            Input_Processing(std::copysign(MAX_INPUT, -(x - X_INIT)), 0, 0, x, y, z, eta);//Copysign(x, y) returns x*(sign of y)
        }
        else if (z < Z_INIT - MAX_INPUT || z > Z_INIT + MAX_INPUT) {
            Input_Processing(0, 0, std::copysign(MAX_INPUT, -(z - Z_INIT)), x, y, z, eta);
        }
        else if (y < Y_INIT - MAX_INPUT || y > Y_INIT + MAX_INPUT) {
            Input_Processing(0, std::copysign(MAX_ROTATION, -(y - Y_INIT)), 0, x, y, z, eta);
        }


        else {
            IsReset = false;

        }
    }
} //r�initialise axe par axe la position du robot. Potentiellement directement par angle plut�t que par position (seulement avec une commande en angle). 
//Un fois la r�initialisation effectu�e, rend False le bool de r�initialisation pour sortir de la boucle.


int Test_Input_Recognition(const char x) {

    if (x == 'z') { //' pour les caract�res, " pour les cha�nes
        return avant;
    }

    if (x == 'q') {
        return gauche;
    }

    if (x == 'd') {
        return droite;
    }

    if (x == 's') {
        return arriere;
    }

    if (x == ' ') {
        return clic;
    }

    if (x == 'p') {
        return end;
    }

    if (x == 'r') {
        return ResetDigit;
    }

    else {
        return unknown;
    }

}//R�cup�re un caract�re et le traduit en int pour les commandes
 // Reconna�t les inputs cardinaux : non applicable pour la suite. Sert de test.

void Test_Input_Translator(int command, bool click, float& X, float& Y, float& Z,  float& eta) {

    float x = X;
    float y = Y;
    float z = Z;
    //copie � passer en argument

    if (!click) {
        if (command == avant) { //avant
           
            Input_Processing(MAX_INPUT, 0, 0, x, y, z, eta);
        }
        else if (command == arriere) { //Arri�re
           
            Input_Processing(-MAX_INPUT, 0, 0, x, y, z, eta);
        }
    }
    else {
        if (command == avant) {
            
            Input_Processing(0, 0, MAX_INPUT, x, y, z, eta);
        }
        else if (command == arriere) {
           
            Input_Processing(0, 0, -MAX_INPUT, x, y, z, eta);
        }
    }

    if (command == gauche) {
      
        Input_Processing(0, -MAX_INPUT, 0, x, y, z, eta);
    }
    else if (command == droite) {
        
        Input_Processing(0, MAX_INPUT, 0, x, y, z, eta);
    };
    //seuls x, y et z sont modifi�s : on retransmet les modifications sur X, Y et Z


    X = x;
    Y = y;
    Z = z; //Mise � jour (peut �tre pas n�cesaire)

}//used for tests with keyboard to translate keyboards inputs in motor inputs. 



void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z,  float &eta) //x : avant arri�re, y = gauche droite et z = haut bas. 
//Input parce que la valeur re�ue peut varier. Les valeurs varient entre 0 et MAX_INPUT et d�pendent de la manette. 
//N�cessit� de prendre l'adresse pour pouvoir modifier en m�me temps. 
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
        float r_Z = (arm * sin(alpha) + forearm * sin(gamma)) - Z;
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
        Y += input_y;


        //variation de eta = etaprec - etanow soit etaprec + gamma pour pouvoir mettre � jour le moteur.
        
        float eta_var = eta + gamma;
        //faire tourner le moteur comme il se doit avec Eta_var


        eta = - gamma; //mise � jour de eta

        //tests
        x = X + wrist * cos(alpha + beta + eta);
        y = Y;
        z = Z + wrist * sin(alpha + beta + eta);

        //positions d'affichage
        int x_elbow =  (X_SHOULDER + renderScaling * arm * cos(alpha));
        int z_elbow =  (Z_SHOULDER - renderScaling * arm * sin(alpha));

        int x_wrist =  (X_SHOULDER + renderScaling * X);
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
        




        
        std::cout << "\nCoordonnees du poignet\n" << std::setprecision(1) << std::fixed << std::setw(4) << x << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << y << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << z <<std::endl;

            

    }
}//Recoit les inputs, traduit les inputs en modification de position et calcule les commandes n�cessaires pour effecuer ce changement. 



float exact_answer(float x, float z) {

    float X = (arm * arm - forearm * forearm + x * x + z * z) / (2 * arm);
    float length = sqrt(x * x + z * z);

    float phi = acos(z / length);

    float blub = asin(X / length);

    float alpha = blub - phi; //La solution qui marche (l'autre ne parche pas pour z n�gatif


    return alpha;

}; //precision, x et y et alpha_pr�c�dent




int main(int argc, char** argv) {


    int stop = 0; //Pour la boucle

    
    bool click_mode = false; //Mode clic d�sactiv� par d�faut

    float x = X_INIT;
    float y = Y_INIT;
    float z = Z_INIT; //les trois positions cardinales qui seront utilis�es pour le robot.
    //Pour les tests, x, y et z iront de 0 � 999, � la premi�re d�cimale

    //ouvrir la fen�tre


    



    float eta = ETA_INIT;

    std::cout << "Press p to exit. Recognized inputs are z, q, s, d, r, and spacebar.\n"
        <<"Use z and s to go forward and backwards\n"
        <<"Use q and d to turn left and right\n"
        <<"Use spacebar to change mode and be a ble to use z and s to move up and down instead\n"
        << "Press r to reset the system to its original position.\n"
        << "Displayed values are in centimeters for distances and arbitrary for angles.\n" 
        <<"First displayed coordinate is the forward axis, then the rotation axis, and then the vertical axis.\n"
        << std::endl;


    std::cout << "\n\n" << Z_SHOULDER << "\n\n";


    bool IsReset = true; //R�initialisation initiale avant de pouvoir agir
    int input = ResetDigit;


    char coarse = 'r';

    //Initialisation de l'affichage
    


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Surface* g_screenSurface;
    SDL_Window* window;


    window = SDL_CreateWindow("Affichage", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN); //Cr�ation de la fen�tre vid�o




    

    
    
    if (window == NULL) {
        printf("Impossible de cr�er l'affichage de dimensions %dx%d : %s\n", WIDTH, HEIGHT, SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    
    sdlRenderer = SDL_CreateRenderer(window, -1, 0); //Cr�ation d'un truc pour render
    
    if (sdlRenderer == NULL) {
        printf("Impossible de cr�er le renderer de dimensions %dx%d : %s\n", WIDTH, HEIGHT, SDL_GetError());
        SDL_Quit();
        return -1;
    }

    while (stop == 0) {
        
        //Verrouillage de l'�cran
        /*if (SDL_MUSTLOCK(g_screenSurface)) {
            if (SDL_LockSurface(g_screenSurface) < 0) {
                printf("Can't lock screen: %s\n", SDL_GetError());
                return -1;
            }
            std::cout << "Screen locked\n";
        }*/

        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);//sdlrenderer d�fini depuis afichage.h
        
        SDL_RenderClear(sdlRenderer);
        



        
        renderThickLine(X_BASE, Z_BASE, X_SHOULDER, Z_SHOULDER);
        renderPivot(X_BASE, Z_BASE);
        renderPivot(X_SHOULDER, Z_SHOULDER);


        if (!IsReset) coarse = getch(); //R�cup�rer un caract�re unique de la console
        input = Test_Input_Recognition(coarse);// traduire le caract�re en digit associ� � un input

        //std::cout << "\nL'input est : " << input << std::endl;

        if (input == end) { //Comande d'arr�t
            std::cout << "\nEnd of Programm" << std::endl;
            SDL_Quit();
            //Fermet la fen�tre
            return 0;
        }
        else if (input == unknown) { //commande inconnue
            std::cout << "\nSomething Unknown was given.\n";
        }
        else if (input == ResetDigit || IsReset) { //commande de r�initialisation, valid�e si l'input est correct ou si le processus est d�j� en cours. 
            IsReset = true;
            reset(IsReset, x, y, z, eta);
        }
        else if (input == clic) {
            if (click_mode) click_mode = false;
            else click_mode = true;
        }
        else { //input reconnu


            //std::cout << "\nCalling Translator\n";
            Test_Input_Translator(input, click_mode, x, y, z, eta);

            
            //Ne pas mettre de \n, sinon le saut de ligne annule en partie le \r
            //Traiter les inputs ici
            IsReset = false; //Interruption du processus de reset si une commande est donn�e.
        }
        std::cout << "\r" << std::setprecision(1) << std::fixed << std::setw(4) << x << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << y << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << z;

        
        
        /*if (SDL_MUSTLOCK(g_screenSurface)) {
            SDL_UnlockSurface(g_screenSurface);
        }*/

        SDL_RenderPresent(sdlRenderer);//mise � jour de l'�cran

        _sleep(frame); // petit d�lai pour ne pas surcharger les inputs. 

    }



}