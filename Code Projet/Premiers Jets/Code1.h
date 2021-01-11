

//FONCTIONS
void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float &alpha_prec);

void reset(bool &IsReset, float& x, float& y, float& z, float &alpha_prec);

int Test_Input_Recognition(const char x);

void Test_Input_Translator(int command, bool click, float& x, float& y, float& z, float &alpha_prec);

float newton(float prec, float x, float z, float &alpha_prec);

float exact_answer(float x, float z, float& alpha_prec);

//Variables

//Corresponff à la position de Reset et à la position Initiale
int X_INIT = 10;
int Y_INIT = 0;
int Z_INIT = 0;
float ETA_INIT = 0;


float frame = 10; //durée de base des inputs en millisecondes. Le cycle a lieu chaque frame, mais la durée des commandes une fois effectuées sera un peu plus longue pour assurer la fluidité, bien qu'elle devra pouvoir être réécrite.  
//A titre de comparaison, une frame dans un JV dure 16.6 ms à 60fps. 
//La valeur de 50 est juste là pour les tests et il serait raisonnable de descendre à 16 ou moins. 

float MAX_INPUT = 60*frame/1000; //Unité de déplacement en cm par input. 60 cm par seconde

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

int wrist = 15;

float pi = 3.14159;


