#include "arm.h"
#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>
#include <stdio.h>


// Données globales
float fps = 60; // En Hz

int Max_speed = 60;//En cm par seconde

float MAX_INPUT = Max_speed / fps;//d�lacement max par frame/période
float MAX_ROTATION_INPUT = 1.2 / fps; //radian.s-1 de rotation de l'axe vertical, arbitraire






// Lancée au démarrage de la machine, permet également de l'initialiser en position.
// L'initialisation dépend de la position de la machine au démarrage afin d'éviter des erreurs entre la réalité et les données de la machine.
Arm::Arm(float new_arm, float new_forearm, float new_wrist){
    arm = new_arm;
    forearm = new_forearm;
    wrist = new_wrist;

    x = 5;
    y = 0;
    z = 0;
    // Initialisation du bras à une position : 5 cm devant l'épaule. 
    
    input_x = 0;
    input_y = 0;
    input_z = 0;

    alpha= 0; 
    beta=0;
    eta=0;

    angle_processing();
    // => initialise les angles vis à vis de la position actuelle. 
    // Alpha, beta et eta sont ainsi mis à jour.

    // NOTE EXTREMEMENT IMPORTANTE : LE ROBOT DOIT ETRE CALIBRE AVANT UTILISATION.
    // LES CAPTEURS SONT DES CAPTEURS DE VITESSE ET NON DE POSITION. PAR CONSEQUENT IL N'EST PAS POSSIBLE DE SE BASER SUR LEUR VALEUR POUR AVOIR LA POSITION D'ORIGINE.
    // POUR CELA IL FAUT S'ASSURER AU LANCEMENT DU ROBOT QUE LA POSITION CITEE PLUS HAUT COMME POSITION DE DEPART EST BIEN LA POSITION DU ROBOT AU DEPART, SINON LE ROBOT VA AGIR N'IMPORTE COMMENT



    // captor_update();
    // alpha = alpha_p;
    // beta = beta_p;
    // eta = eta_p;

    // x = (arm * cos(alpha) + forearm * cos(alpha + beta));
    // z = (arm * sin(alpha) + forearm * sin(alpha + beta)); 
    // y = atan(2*length/input_y);

    // is_reseting = true;
    // reset();
    // On reset la machine à une position initiale.
    // Ne fonctionne pas car les capteurs sont des capteurs de vitesse


};



void Arm::input_processing(){
    
    if (!(length > arm + forearm - MAX_INPUT || x < 0)) { 
        //vérification qu'il n'est pas possible d'aller plus loin que ce qui est possible, par exemple x<0 est non autorisé.
        // Pourquoi le Max_Input ? Sert de garde ?

        x_p = x;
        y_p = y;
        z_p = z;
        // Sauvegarde des anciens x, y et z. Potentiellement effectué dans captor_update réalisé auparavant. (non)

        x = (arm * cos(cpt_alpha) + forearm * cos(cpt_alpha + cpt_beta));
        z = (arm * sin(cpt_alpha) + forearm * sin(cpt_alpha + cpt_beta)); 
        // On met à jour la position actuelle en fonction des positions données par les capteurs pour éviter les erreurs
        // On a ainsi la position exacte du robot.

        x = x + input_x;
        y = y + input_y;
        z = z + input_z;
        // Mise à jour des x, y et z désirés. A ce stade, ces valeurs sont temporaires et les valeurs qu'on va stocker plus tard seront celles de capteurs. 


        float length = sqrt(x * x + z * z);
        // Calcul de la longueur entre le poignet et l'épaule. Permet entre autres choses de vérifier qu'on ne demande pas un acte impossible au bras.

        angle_processing();
        // Mise à jour des angles compte tenu de la position désirée.

        x = x_p + (arm * cos(alpha) + forearm * cos(alpha + beta));
        z = z_p +  (arm * sin(alpha) + forearm * sin(alpha + beta)); 
        y = y_p + atan(2*length/input_y);
        // On utilise maintenant les valeurs du programme qui ont été calculées dans angle_processing : x, y et z sont désormais des valeurs objectif à atteindre par le programme.

    
    }
    // Si les conditions ne sont pas respectées, on en fait rien du tout, le bras est immobile tant que le mouvement demandé est impossible. Aucune variable n'est alors modifiée.
}


void Arm::angle_processing(){

    alpha_p = alpha;
    beta_p = beta;
    eta_p = eta;
    // Sauvegarde des anciens angles. Potentiellement inutile. 


    float X = (arm * arm - forearm * forearm + x * x + z * z) / (2 * arm);

    float length = sqrt(x * x + z * z);
    // Longueur du bras

    float phi = acos(z / length);
    float xi = asin(X / length);
    // Angles

    alpha = xi - phi; 
    //La solution qui marche (l'autre ne parche pas pour z négatif)


    double inside_c = (x - arm * cos(alpha)) / forearm; 
    double inside_s = (z - arm * sin(alpha)) / forearm;
    // Ce qui va être dans arccos et arcsin pour la suite. 


    if (inside_c > sqrt(2) / 2 || z < -30) { //empirique : lorsque inside_c = sqrt(2)/2, inside_s aussi et � ces valeurs, arcsin et arccos sont �gaaux : on peut passer d el'un a� l'autre
        //correspond � z partiellement n�gatif
        // Probl�me : pour z sufisamment bas, inside_c repasse en dessous de acine2/2, donc on va poser un palier arbitraire dans lequel on utilisera toujours arcsin.
        // Ce palier a été décidé empiriquement.

        if (inside_s < 0.9999 || inside_s > -0.9999) {
            
            beta = asin(inside_s) - alpha;
        }
        else {
            beta = -alpha;
        }
    }
    //pas de situations ou le contenu de arccos est inf�rieur � -1 (ou m�me � 0) car pas de x n�gatif
    else {
        
        beta = acos(inside_c) - alpha;
    }

    eta -= beta;
    // Pour maintenir le poignet dans le même alignement, il faut lui retirer la valeur de modification de beta. 

}

void Arm::serial_processing(char* str){
    
    
    char* format = "Joystick X = %f, Joystick Y = %f, Button is %d, Reset is %d, joystick Max is : %f";
    // X est l'axe horizontal, Y vertical.
    // Format du string qu'il doit y avoir en entrée. X et Y valent une valeur quelconque entre des bornes définies par Max.
    // Button sert à déterminer si on est en mode click ou non click, ce qui détermine comment on se sert du bras.
    // Idéalement max est reçu une seule fois puis est fixé. Ou encore mieux, fixé des deux côtés sans avoir à faire de sérial...

    float X, Y, max;
    int B, R;

    if (is_reseting){
        sscanf(format, str, &X, &Y, &B, &R, &max);

        input_y = X/max * MAX_ROTATION_INPUT;
        // La rotation latérale de l'épaule est déterminée par l'axe horizontal du joystick.

        if(B==0){ // Mode cliqué : X dirige d'avant en arrière.
            input_x = Y/max * MAX_INPUT;
            input_z = 0;
            // Bien réinitialiser input_z à zéro ! Sinon il gardera sa valeur précédente.
        }
        else{
            input_z = Y/max * MAX_INPUT;
            input_x = 0;
        }

        if (R == 1) {
            reset();
        }
    }
    
    // Si la machine est e train de reset, aucune commande ne sera lue tant qu'elle n'a pas fini de le faire.
    

}


int signof(float a){
    if (a<0){
        return -1;
    }
    else{
        return 1;
    }
}

void Arm::reset(){

    int X_INIT = 10;
    int Y_INIT = 0;
    int Z_INIT = 0;

    if (is_reseting) { //If pas nécessaire, puisque cette vérification sera sans doute faite avant l'appel de la fonction.
            

        if (x < X_INIT - MAX_INPUT || x > X_INIT + MAX_INPUT) {
            input_x = MAX_INPUT/2 * signof(-(x - X_INIT))
            input_y = 0;
            input_z = 0;
            Input_Processing();
        }
        else if (z < Z_INIT - MAX_INPUT || z > Z_INIT + MAX_INPUT) {
            input_z = MAX_INPUT/2 * signof(-(z - Z_INIT));
            input_y = 0;
            input_x = 0;
            Input_Processing();
        }
        else if (y < Y_INIT - MAX_INPUT || y > Y_INIT + MAX_INPUT) {
            input_y = MAX_ROTATION_INPUT/2 * signof(-(y - Y_INIT));
            input_x = 0;
            input_z = 0;
            Input_Processing();
        }


        else {
            is_reseting = false;
            //std::cout << "Reset done.\n";
        }
    }


}

void Arm::captor_update(float alpha_vit, float beta_vit){

    cpt_alpha += alpha_vit/1000;
    cpt_beta += beta_vit/1000;



    // Des variables globales devront être définies pour définir le 0 des capteurs. En effet le capteur ne donnera pas nécessairement 0 à son démarrage, mais
    // un angle actuel selon son paramétrage. Il faudra cette valeur pour définir proprement le 0 ainsi que les angles par défauts, qui donnent ensuite la position.
}