#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>
#include 'arm.h'


// #DEFINE ENCODEROUTPUT 64*4


// Constants 

const int serialInput = 0;

const int enableBridge1 = 22;
const int enableBridge2 = 23;
// Initialisation des ponts en H

const int shoulderMotorDirect = 3;
const int shoulderMotorIndirect = 4;
const int elbowMotorDirect = 5;
const int elbowMotorIndirect = 6;
// Déclaration des MCC


Stepper stepperMotor(numberofstep,24,25,26,27);
// Déclaration du stepper


const int shoulder_cpt = 28;
const int elbow_cpt = 29;

int shoulderEncoderValue = 0;
int elbowEncoderValue = 0;
// Compteurs
float cpt_shoulder_vit = 0;
float cpt_elbow_vit = 0;
const int encoder_PPR = 4*64;
// Déclaration des capteurs et des données :
// Ports, valeur du capteur, et stockage des vitesses en rad.s-1
// L'encodeur a un Count per Revolution de 64 donc un Pulse per Revolution de 4*64.



const float arm = 25;
const float foreArm = 10;
const float wrist = 10;
// Déclaration des tailles des membres

const int freq = 60;
// les inputs sont reçus de l'application à une fréquence de 60 Hz. 


// const int Tour=0; // Déclaration variable nombre de tour
// const int delayTime=8; // Déclaration variable vitesse du moteur



// Variables
char buf[1024]; 
int cur = 0;
// Buffer qui contiendra le message serial reçu. Ce message pourrait être incomplet vu que l'arduino tourne très vite.
// Cur est la position du curseur qui parcourt le message

float vit_alpha = 0;
float vit_beta = 0;
float vit_rotation = 0;
// Vitesses/puissances demandées aux moteurs des angles alpha et beta.

//variables de temps pour les capteurs
float interval = 1/1000;
// Les capteurs vont tourner à 1000 Hz
long previousMillis = 0;
long currentMillis = 0;
// pour compter le temps


// Déclaration du Robot
Arm* robot = new Arm(arm, foreArm, wrist);




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
    stepperMotor.setSpeed(20);

}



void loop{

    // Initialisation
    digitalWrite(enableBridge1,HIGH); // Active pont en H
    digitalWrite(enableBridge2,HIGH); // Active pont en H


    // Bloc récupération des données capteurs. A lancer avant la réception des inputs serial.
    // Boucle pour les capteurs

    currentMillis = millis();

    if ( (currentMillis - previousMillis) > interval) {
        
        previousMillis = currentMillis;


        // vitesse de rotation =
        // (total encoder pulse in 1/1000s / motor encoder output) x 1000ms / 2*pi
        cpt_shoulder_vit = (float)(shoulderEncoderValue * 1000 / (2 * 3.141592 * encoder_PPR));
        cpt_elbow_vit = (float)(elbowEncoderValue * 1000 / (2 * 3.141592 * encoder_PPR));

        robot.captor_update(cpt_shoulder_vit, cpt_elbow_vit);

        // Only update display when there have readings
        // if ( rpm > 0) {
        // Serial.print(encoderValue);
        // Serial.print(" pulse / ");
        // Serial.print(ENCODEROUTPUT);
        // Serial.print(" pulse per rotation x 60 seconds = ");
        // Serial.print(rpm);
        // Serial.println(" RPM");
        // }
        
        shoulderEncoderValue = 0;
        elbowShoulderValue = 0;
    }

    // Réception des données

    if (Serial.available())
    {
        buf[cur] = Serial.read();
        if (buf[cur] != -1)
            cur++;
        buf[cur] = '\0';
        //Serial.print(buf[cur - 1]); //debug
        if (buf[cur - 1] == '\n' || buf[cur - 1] == '\r')
            {
            // Déclenché si on repère qu'on a un serial complet
            robot.serial_processing(buf);
            // Processing du serial en données utilisables
            
            buf='';
            cur=;
            // Réinitialisation du buffer.

            robot.input_processing();
            // Processing des données pour obtenir la position voulue et les angles voulus (appelle angle processing)
            
        }
    }

    // A cette étape, on a soit les nouvelles données pour x, y, z et les angles, soit les données précédentes. On peut donc procéder au mouvement.

    

    // Mouvements et asservissement

    // Asservissement : 
    // Si la vitesse mesurée est différente de la vitesse voulue (l'angle multiplié par la fréquence d'envoi des commandes), on modifie légèrement l'ordre donné aux moteurs
    if (robot.cpt_alpha < robot.alpha*60){
        vit_alpha += 0.1;
    }
    else {
        vit_alpha -= 0.1;
    }

    if (robot.cpt_beta < robot.beta*60){
        vit_beta += 0.1;
    }
    else {
        vit_beta -=0.1;
    }


    // Activation des moteurs avec la puissance donnée :
    
    activate_shoulder();

    activate_elbow();

    activate_rotation();


}


// Fonctions pour activer les moteurs

void activate_shoulder(){
    if (vit_alpha>255){
        vit_alpha = 255;
    }
    else if (vit_alpha<-255){
        vit_alpha = -255;
    }
    if (vit_alpha > 0){
        analogWrite(shoulderMotorDirect, vit_alpha);
        analogWrite(shoulderMotorIndirect,0);
    }
    else{
        analogWrite(shoulderMotorIndirect, -vit_alpha);
        analogWrite(shoulderMotorDirect,0);
    }
}

void activate_elbow(){
    if (vit_beta > 255){
        vit_beta = 255;
    }
    else if (vit_beta < -255){
        vit_beta = -255
    }
    if (vit_beta > 0){
        analogWrite(elbowMotorDirect,vit_beta);
        analogWrite(elbowMotorIndirect,0);
    }
    else{
        analogWrite(elbowMotorIndirect, -vit_beta);
        analogWrite(elbowMotorDirect,0);
    }
}

void activate_rotation(){
    int rapp = 10;
    // multiplicateur entre le nombre de tours et l'input. A modifier pour avoir la valeur qu'on veut.
    
    stepperMotor.setSpeed(rapp * robot.input_y);
    stepperMotor.step(500);
    // Tourne 500 steps avec une vitesse dépendant de l'input_y.

}


// Fonctions pour activer les capteurs

void EncoderInit()
{
 // Attach interrupt at hall sensor A on each rising signal
  attachInterrupt(digitalPinToInterrupt(shoulder_cpt), updateShoulderEncoder, RISING);
  attachInterrupt(digitalPinToInterrupt(elbow_cpt), updateElbowEncoder, RISING);
}

void updateShoulderEncoder()
{
  // Add encoderValue by 1, each time it detects rising signal
  shoulderEncoderValue++;
}

void updateElbowEncoder()
{
  // Add encoderValue by 1, each time it detects rising signal
  elbowEncoderValue++;
}