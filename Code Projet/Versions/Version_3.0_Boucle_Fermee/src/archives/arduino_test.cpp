// // Constants
// const int enableBridge1 = 2;
// const int MotorForward1 = 3;
// const int MotorReverse1 = 5;
// //Correspond aux numéros des entrées/sorties. L'entrée pour faire tourner le moteur vers l'avant est 3, pour le faire reculer, 5.

// // Variables
// int Power = 80; //Le rapport cyclique, entre 0 et 255. à 127, en théorie, le moteur est à l'arrêt. 

// void setup(){
//   pinMode(MotorForward1,OUTPUT);
//   pinMode(MotorReverse1,OUTPUT);
//   pinMode(enableBridge1,OUTPUT);
// //   Initialisation et mise en lien des constantes avec leur rôle. Cette fonction est importante.

// }

// void loop(){
//   digitalWrite(enableBridge1,HIGH); // Active pont en H

//   // Tourne dans le sens direct pendant 2 secondes
//   analogWrite(MotorForward1,Power);
//   analogWrite(MotorReverse1,0);
//   delay(2000);
  
//   // Tourne dans le sens indirect pendant 3 secondes
//   analogWrite(MotorForward1,0);
//   analogWrite(MotorReverse1,Power);
//   delay(3000);
  
//   //Arrête le moteur pendant 1 seconde
//   analogWrite(MotorForward1,0);
//   analogWrite(MotorReverse1,0);
//   digitalWrite(enableBridge1,LOW); 
//   delay(1000);
// }





// Boucle de serial de Wendrul
//    if (Serial.available())
//     {
//         digitalWrite(7, HIGH); //à Ignorer
//         buf[cur] = Serial.read(); //Récupération du serial
//         if (buf[cur] != -1)
//             cur++; //Mise à jour du curseur
//         buf[cur] = '\0'; //On écrit sur le curseur un \0
//         //Serial.print(buf[cur - 1]); //debug, à ignorer
//         if (buf[cur - 1] == '\n' || buf[cur - 1] == '\r') //si on a reçu une fin de ligne :
//         {
//             digitalWrite(7, LOW); 
//             face->ReadSerialInstruction(buf); //Instructions à réaliser sur Face via ReadSerialInstruction(buf)

//             ClearBuf();
//             UpdatePos();
//         }



#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>
// #include <Serial.h>



// Constants 

const int serialInput = 0;

const int enableBridge1 = 1;
const int enableBridge2 = 2;
// Initialisation des ponts en H

const int shoulderMotorDirect = 3;
const int shoulderMotorIndirect = 4;
const int elbowMotorDirect = 5;
const int elbowMotorIndirect = 6;

// Déclaration des MCC

// const int Wrist1= 8; // Déclaration commande bobine A moteur
// const int Wrist2= 9; // Déclaration commande bobine B moteur
// const int Wrist3= 10; // Déclaration commande bobine C moteur
// const int Wrist4= 11; // Déclaration commande bobine D moteur
Stepper stepperMotor(numberofstep,9,11,10,6);
// Déclaration du stepper

const int Tour=0; // Déclaration variable nombre de tour
const int delayTime=8; // Déclaration variable vitesse du moteur

// Variables
char buf[1024]; 
// Buffer qui contiendra le message serial reçu. Ce message pourrait être incomplet vu que l'arduino tourne très vite.


void setup{
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

    pinMode(shoulderMotorDirect, OUTPUT);
    pinMode(shoulderMotorIndirect, OUTPUT);
    pinMode(elbowMotorDirect, OUTPUT);
    pinMode(elbowMotorIndirect, OUTPUT);
    // Init des mcc

    // pinMode(MotorPin1, OUTPUT); // Affectation de la sortie digitale n°8
    // pinMode(MotorPin2, OUTPUT); // Affectation de la sortie digitale n°9
    // pinMode(MotorPin3, OUTPUT); // Affectation de la sortie digitale n°10
    // pinMode(MotorPin4, OUTPUT); // Affectation de la sortie digitale n°11
    // Init du Stepper
    motor.setSpeed(20);

}

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
}//Reset les positions avec le robot.


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
        float r_Y = input_y / x;
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

void loop{

    // Initialisation
    digitalWrite(enableBridge1,HIGH); // Active pont en H
    digitalWrite(enableBridge2,HIGH); // Active pont en H

    // Réception des données

    

    // Mouvements

    motor.step(-600); // the motor will run during 600 step (in reverse ?) (you can change the step)


}