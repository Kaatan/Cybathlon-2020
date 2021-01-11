//Code de test
#include <cstdio>
#include <stdlib.h> //contient sleep
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <iomanip>

#include <conio.h> //nécessaire pour recevoir les inputs de la console sans validation

#include "Code1.h"


//###################################################################################
// ###########   CE CODE SERVIRA DE TEMPLATE POUR LA SUITE   ########################
// IL CONTIENT LE PRINCIPE DE BASE POUR L'ACQUISITION DES INPUTS VIA SLEEP ET CONIO #
//###################################################################################




void reset(bool& IsReset, float& x, float& y, float& z, float& alpha_prec) {
    if (IsReset) {

        if (x < X_INIT - MAX_INPUT || x > X_INIT + MAX_INPUT) {
            Input_Processing(std::copysign(MAX_INPUT, -(x - X_INIT)), 0, 0, x, y, z, alpha_prec);//Copysign(x, y) returns x*(sign of y)
        }
        else if (z < Z_INIT - MAX_INPUT || z > Z_INIT + MAX_INPUT) {
            Input_Processing(0, 0, std::copysign(MAX_INPUT, -(z - Z_INIT)), x, y, z, alpha_prec);
        }
        else if (y < Y_INIT - MAX_INPUT || y > Y_INIT + MAX_INPUT) {
            Input_Processing(0, std::copysign(MAX_ROTATION, -(y - Y_INIT)), 0, x, y, z, alpha_prec);
        }


        else {
            IsReset = false;

        }
    }
} //réinitialise axe par axe la position du robot. Potentiellement directement par angle plutôt que par position (seulement avec une commande en angle). 
//Un fois la réinitialisation effectuée, rend False le bool de réinitialisation pour sortir de la boucle.


int Test_Input_Recognition(const char x) {

    if (x == 'z') { //' pour les caractères, " pour les chaînes
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

}//Récupère un caractère et le traduit en int pour les commandes
 // Reconnaît les inputs cardinaux : non applicable pour la suite. Sert de test.

void Test_Input_Translator(int command, bool click, float& X, float& Y, float& Z, float& alpha_prec) {

    float x = X;
    float y = Y;
    float z = Z;
    //copie à passer en argument

    if (!click) {
        if (command == avant) { //avant
           
            Input_Processing(MAX_INPUT, 0, 0, x, y, z, alpha_prec);
        }
        else if (command == arriere) { //Arrière
           
            Input_Processing(-MAX_INPUT, 0, 0, x, y, z, alpha_prec);
        }
    }
    else {
        if (command == avant) {
            
            Input_Processing(0, 0, MAX_INPUT, x, y, z, alpha_prec);
        }
        else if (command == arriere) {
           
            Input_Processing(0, 0, -MAX_INPUT, x, y, z, alpha_prec);
        }
    }

    if (command == gauche) {
      
        Input_Processing(0, -MAX_INPUT, 0, x, y, z, alpha_prec);
    }
    else if (command == droite) {
        
        Input_Processing(0, MAX_INPUT, 0, x, y, z, alpha_prec);
    };
    //seuls x, y et z sont modifiés : on retransmet les modifications sur X, Y et Z


    X = x;
    Y = y;
    Z = z; //Mise à jour (peut être pas nécesaire)

}//used for tests with keyboard to translate keyboards inputs in motor inputs. 



void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float& alpha_prec) //x : avant arrière, y = gauche droite et z = haut bas. 
//Input parce que la valeur reçue peut varier. Les valeurs varient entre 0 et MAX_INPUT et dépendent de la manette. 
//Nécessité de prendre l'adresse pour pouvoir modifier en même temps. 
{

    //mise à jour des coordonnées
    float x = X + input_x;
    float y = Y + input_y;
    float z = Z + input_z;
    


    float length = sqrt(x * x + z * z); //calcul de la taille totale en cm
    
    if (length > arm + forearm - MAX_INPUT || x < 0) { //vérification qu'il n'est pas possible d'aller plus loin. 
        //Problème actuel : bloque tous les mouvements, pas juste celui qui pose problème. Un peu rigide. 
        std::cout << "\nMax position reached, can't go any further\n" << std::endl;

    }
    else {


       
        float alpha = exact_answer(x, z, alpha_prec);

        //std::cout << "\nFinal alpha is " << alpha << " in radians\n";


        //Definitions
        float beta;
        float gamma;



        double inside_c = (x - arm * cos(alpha)) / forearm; // c for cos
        double inside_s = (z - arm * sin(alpha)) / forearm;

        

        if (inside_c > sqrt(2) / 2 || z < -30) { //empirique : lorsque inside_c = sqrt(2)/2, inside_s aussi et à ces valeurs, arcsin et arccos sont égaaux : on peut passer d el'un aà l'autre
            //correspond à z partiellement négatif
            //Problème : pour z sufisamment bas, inside_c repasse en dessous de acine2/2, donc on va poser un palier arbitraire dans lequel on utilisera toujours arcsin.
            
            if (inside_s < 0.9999 || inside_s > -0.9999) {
                
                gamma = asin(inside_s);
                beta = gamma - alpha;
            }
            else {
                beta = -alpha;
            }

        }
        
        //pas de situations ou le contenu de arccos est inférieur à -1 (ou même à 0) car pas de x négatif
        
        else {
            
            gamma = acos(inside_c);

            beta = gamma - alpha;
        }

        // Attention au domaine de définition de Acos espèce de bêta !!!
        //Beta est nécesaire aux moteurs



        //faire quelque chose avec les angles, typiquement les donner commme arguments en vitesse dans un moteur.
        //Dans cette fonction ou une autre ?

        //Pas besoin du bool Click : le fait de savoir si on va vers le haut ou l'avant sera déterminé avant au moment de mettre les inputs en arguments. 


        //Update motors selon x, y et z



        float r_X = (arm * cos(alpha) + forearm * cos(gamma)) - X;
        float r_Z = (arm * sin(alpha) + forearm * sin(gamma)) - Z;
        //gamma est l'angle du second axe par rapport à l'horizontal, pas beta. Il faut donc prendre gamma pour calculer les cosinus. 

        //std::cout << "Translation on X due to first motor is  " << arm * cos(alpha) 
         //   << "\nTranslation on X due to second motor is  " << forearm * cos(gamma);

        //std::cout << "\nTrue input value changes are " << r_X << " for the horizontal plan,  " << input_y << " for the rotationnal plan and " << r_Z <<" for the vertical plan\n";

        //Mise à jour des coordonnées : faire touner en fonction de l'angle. Comme la commande est en vitesse, il faudra faire tourner le moteur
        //à une certaine vitesse pendant une frame. La vitesse dépendra de la distance finale à parcourir, mais également de la position actuelle.

        //attention ! la vitesse de rotation ne pourra pas être constante pour toute extension. Lorsque le poignet
        //est très près, la moindre rotation aura une grande influence. Il faudra dont en quelque sorte multiplier par une fonction 
        //croissante de la distance. Tests à réaliser. 


        X += r_X;
        Z += r_Z;
        Y += input_y;

        alpha_prec = alpha;
        //mise à jour de alpha prec

        //std::cout << "\nFinal calculated values are " << X << " " << Y << " " << Z<<std::endl;

    }//juste pour la validation et terminer le programme. 
}//Recoit les inputs, traduit les inputs en modification de position et calcule les commandes nécessaires pour effecuer ce changement. 



float exact_answer(float x, float z, float& alpha_prec) {

    float X = (arm * arm - forearm * forearm + x * x + z * z) / (2 * arm);
    float length = sqrt(x * x + z * z);

    float phi = acos(z / length);

    float blub = asin(X / length);

    float alpha = blub - phi; //La solution qui marche (l'autre ne parche pas pour z négatif


    return alpha;

}; //precision, x et y et alpha_précédent



int main() {


    int stop = 0;

    bool IsReset = false;
    bool click_mode = false; //Mode clic désactivé par défaut

    float alpha_prec = -pi / 2 + 0.1; //le alpha initial est très proche de -pi/2 

    float x = X_INIT;
    float y = Y_INIT;
    float z = Z_INIT; //les trois positions cardinales qui seront utilisées pour le robot.
    //Pour les tests, x, y et z iront de 0 à 999, à la première décimale

    std::cout << "Press p to exit. Recognized inputs are z, q, s, d, r, and spacebar.\n"
        <<"Use z and s to go forward and backwards\n"
        <<"Use q and d to turn left and right\n"
        <<"Use spacebar to change mode and be a ble to use z and s to move up and down instead\n"
        << "Press r to reset the system to its original position.\n"
        << "Displayed values are in centimeters for distances and arbitrary for angles.\n" 
        <<"First displayed coordinate is the forward axis, then the rotation axis, and then the vertical axis.\n"
        << std::endl;

    std::cout << "\r" << std::setprecision(1) << std::fixed << std::setw(4) << x << " "
        << std::setprecision(1) << std::fixed << std::setw(4) << y << " "
        << std::setprecision(1) << std::fixed << std::setw(4) << z;


    char coarse;

    while (stop == 0) {



        if (!IsReset) coarse = getch(); //Récupérer un caractère unique de la console
        int input = Test_Input_Recognition(coarse);// traduire le caractère en digit associé à un input

        //std::cout << "\nL'input est : " << input << std::endl;

        if (input == end) { //Comande d'arrêt
            std::cout << "\nEnd of Programm" << std::endl;
            return 0;
        }
        else if (input == unknown) { //commande inconnue
            std::cout << "\nSomething Unknown was given.\n";
        }
        else if (input == ResetDigit || IsReset) { //commande de réinitialisation, validée si l'input est correct ou si le processus est déjà en cours. 
            IsReset = true;
            reset(IsReset, x, y, z, alpha_prec);
        }
        else if (input == clic) {
            if (click_mode) click_mode = false;
            else click_mode = true;
        }
        else { //input reconnu


            //std::cout << "\nCalling Translator\n";
            Test_Input_Translator(input, click_mode, x, y, z, alpha_prec);

            
            //Ne pas mettre de \n, sinon le saut de ligne annule en partie le \r
            //Traiter les inputs ici
            IsReset = false; //Interruption du processus de reset si une commande est donnée.
        }
        std::cout << "\r" << std::setprecision(1) << std::fixed << std::setw(4) << x << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << y << " "
            << std::setprecision(1) << std::fixed << std::setw(4) << z;



        _sleep(frame); // petit délai pour ne pas surcharger les inputs. 

    }



}