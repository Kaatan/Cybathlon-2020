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




void reset(bool &IsReset, float &x, float &y, float &z, float &alpha_prec) {
    if (IsReset) {
        
        if (x < X_INIT - MAX_INPUT || x > X_INIT + MAX_INPUT) {
            Input_Processing(std::copysign(MAX_INPUT, -(x-X_INIT)), 0, 0, x, y, z, alpha_prec);//Copysign(x, y) returns x*(sign of y)
        }
        else if (z < Z_INIT - MAX_INPUT || z > Z_INIT + MAX_INPUT) {
            Input_Processing(0, 0, std::copysign(MAX_INPUT, -(z-Z_INIT)), x, y, z, alpha_prec);
        }
        else if (y < Y_INIT - MAX_INPUT || y > Y_INIT + MAX_INPUT) {
            Input_Processing(0, std::copysign(MAX_ROTATION, -(y-Y_INIT)), 0, x, y, z, alpha_prec);
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

    else{
        return unknown;
    }
    
}//Récupère un caractère et le traduit en int pour les commandes
 // Reconnaît les inputs cardinaux : non applicable pour la suite. Sert de test.

void Test_Input_Translator(int command, bool click, float &X, float &Y, float &Z, float &alpha_prec) {

    float x = X;
    float y = Y;
    float z = Z;
    //copie à passer en argument

    if (!click) {
        if (command == avant) { //avant
            //std::cout << "\nCalling Processing for X\n";
            Input_Processing(MAX_INPUT, 0, 0, x, y, z, alpha_prec);
        }
        else if (command == arriere) { //Arrière
            //std::cout << "\nCalling Processing for -X\n";
            Input_Processing(- MAX_INPUT, 0, 0, x, y, z,alpha_prec);
        }
    }
    else {
        if (command == avant) {
            //std::cout << "\nCalling Processing for Z\n";
            Input_Processing(0, 0, MAX_INPUT, x, y, z, alpha_prec);
        }
        else if (command == arriere) {
            //std::cout << "\nCalling Processing for -Z\n";
            Input_Processing(0, 0, -MAX_INPUT, x, y, z, alpha_prec);
        }
    }
    
    if (command == gauche) {
        //std::cout << "\nCalling Processing for -Y\n";
        Input_Processing(0, -MAX_INPUT, 0, x, y, z, alpha_prec);
    }
    else if (command == droite) {
        //std::cout << "\nCalling Processing for Y\n";
        Input_Processing(0, MAX_INPUT, 0, x, y, z, alpha_prec);
    };
    //seuls x, y et z sont modifiés : on retransmet les modifications sur X, Y et Z


    X = x;
    Y = y;
    Z = z; //Mise à jour (peut être pas nécesaire)

}//used for tests with keyboard to translate keyboards inputs in motor inputs. 



void Input_Processing(float input_x, float input_y, float input_z, float& X, float& Y, float& Z, float &alpha_prec) //x : avant arrière, y = gauche droite et z = haut bas. 
//Input parce que la valeur reçue peut varier. Les valeurs varient entre 0 et MAX_INPUT et dépendent de la manette. 
//Nécessité de prendre l'adresse pour pouvoir modifier en même temps. 
{

    //mise à jour des coordonnées
    float x = X + input_x;
    float y = Y + input_y;
    float z = Z + input_z;
    //NOTE : Effets de bords (bons dans ce cas précis)
    //x, y et z sont donc mis à jour. 
    /*std::cout << "Received X input : " << input_x
        << "\nReceived Y input : " << input_y
        << "\nReceived Z input : " << input_z;*/

    /*std::cout << "\nInitial non updated values : " << X << " " << Y << " " << Z << std::endl;
    std::cout << "\nPredicted updated values : " << x << " " << y << " " << z << std::endl;*/


    float length = sqrt(x * x + z * z); //calcul de la taille totale en cm
    std::cout << "\n\nExtended length is " << length << std::endl;

    if (length > arm + forearm - MAX_INPUT || x<0) { //vérification qu'il n'est pas possible d'aller plus loin. 
        //Problème actuel : bloque tous les mouvements, pas juste celui qui pose problème. Un peu rigide. 
        std::cout << "\nMax position reached, can't go any further\n" << std::endl;

    }
    else {


        /*float angle = atan(z / x); //calcul de l'angle, non nécessaire. Il s'agit de gamma. 

        //calcul des coordonnées de la position finale voulue. normalement, elles sont égales aux positions réelles finales. 
        float x_l = length * cos(angle);
        float z_l = length * sin(angle);


        std::cout << "L'angle omega voulu est : " << angle / (2 * pi) * 360 << std::endl;
        std::cout << "Les positions finales sont : "<<x_l << " et " << z_l << std::endl;
        */


        //Calcul des angles nécessaires pour pouvoir ensuite calculer la variation souhaitée
        //Résolution numérique

        float prec = 0.1;

        //std::cout << "\nCalling Newton\n";

        //float alpha = newton(prec, x, z, alpha_prec);
        float alpha = exact_answer(x, z, alpha_prec);

        //std::cout << "\nFinal alpha is " << alpha << " in radians\n";


        //Definitions
        float beta;
        float gamma;



        double inside_c = (x - arm * cos(alpha)) / forearm; // c for cos
        double inside_s = (z - arm * sin(alpha)) / forearm;

        std::cout << "Inside of Acos is " << std::setprecision(10) << inside_c<< std::endl;
        std::cout << "Inside of Asin is " << std::setprecision(10) << inside_s << std::endl;



        if (inside_c > sqrt(2)/2  || z<-30) { //empirique : lorsque inside_c = sqrt(2)/2, inside_s aussi et à ces valeurs, arcsin et arccos sont égaaux : on peut passer d el'un aà l'autre
            //correspond à z partiellement négatif
            //Problème : pour z sufisamment bas, inside_c repasse en dessous de acine2/2, donc on va poser un palier arbitraire dans lequel on utilisera toujours arcsin.
            printf("Emergency : inside arccos > 0.9999\n");
            if (inside_s < 0.9999 || inside_s > -0.9999) {
                printf("Using arcsin\n");
                gamma = asin(inside_s);
                beta = gamma - alpha;
            }
            else {
                beta = -alpha;
            }
            
        }
        else if (inside_c < -0.999){ // non utilisé pour le moment
            printf("Emergency : inside accos < 0.9999\n");
            if (inside_s < 0.9999 || inside_s > -0.9999) {
                printf("Using arcsin\n");
                gamma = asin(inside_s); // CF les courbes de arccos et arcsin
                beta = gamma - alpha;
            }
            else {
                beta = pi - alpha;
            }
           
        }
        else {
            printf("Using Default\n");
            gamma = acos(inside_c);

            beta = gamma - alpha;
        }

        // Attention au domaine de définition de Acos espèce de bêta !!!
        //Beta est nécesaire aux moteurs

        std::cout << "Alpha is : " << alpha / (2 * pi) * 360 << "\nBeta is : " << beta / (2 * pi) * 360 
            <<"\nGamma is : " << gamma / (2 * pi) * 360 << std::endl;


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


float newton(float prec, float x, float z, float &alpha_prec) {//en entrée la précision et les valeurs actuelles de x et z (avant et hauteur)

    float alpha = -pi/2+0.1;
    float X = (arm*arm - forearm*forearm + x*x + z*z)/(2*arm);
    
    
    //std::cout << "     Constant of x and z is : " << X << std::endl;


    /*float a = 0;
    float b = 2 * 3.141592;
    //Intervalle de recherche*/

    int iter = 0;
    int max_iter = 200;
    int stop = 0;

    while (iter<max_iter && ((x * cos(alpha) + z * sin(alpha) - X) > prec || (x * cos(alpha) + z * sin(alpha) - X) < -prec)) {
        
        float u = (-x * sin(alpha) + z * cos(alpha)); //dérivée

        if (u == 0) {
            std::cout << "\nWarning null Derivative\n";
            u = 0.5;//arbitraire pour déboguer le programme
        }

        alpha -= ((x * cos(alpha) + z * sin(alpha) - X) / u);
        //Calcul de alpha
        
        /*while (alpha > pi || alpha < -pi) { //modulo 2pi
            if (alpha > pi) {
                alpha -= 2 * pi;//On reste entre -pi et pi
            }
            if (alpha < -pi) {
                alpha += 2 * pi;
            }
        }*/
        
        //Affiche les valeurs trouvées à chaque étape.
        //std::cout << (x * cos(alpha) + z * sin(alpha) - X) << std::endl;

        iter++;
        if (iter == max_iter) {
            std::cout << "     Max iter reached"<<std::endl;
        }
    }

    //la valeur qu'on doit prendre dépend en fait beaucoup de la position
    //Pour des raisons mécaniques il faut prendre la valeur de alpha la plus proche de l'axe vertical négatif
    //=> Il faut prendre les solutions entre -pi/2 et 3pi/2
    //Plus vrai

    //ATTENTION : SI LA HAUTEUR DEVIENT NEGATIVE, LE ALHA A PRENDRE DEVRA ETRE DERRIERE -pi/2
    //=> SEPARER SELON QUE LA HAUTEUR EST POSITIVE OU NEGATIVE
    //ATTENTION : LORSQUE LE BRAS EST ETENDU? LE PROBLEME N'EST PAS

    //Les deux ajustements ne sont potentiellement pas nécessaires
    while (alpha > pi || alpha < -pi) { //modulo 2pi
        if (alpha > pi) {
            alpha -= 2 * pi;//On reste entre -pi/2 et 3pi/2
        }
        if (alpha < -pi) {
            alpha += 2 * pi;
        }
    }

    

    float alpha2 = alpha + pi; //l'autre solution

    if (alpha2 > pi) {
        alpha2 -= 2 * pi;//On reste entre -pi/2 et 3pi/2
    }
    if (alpha2 < -pi) {
        alpha2 += 2 * pi;
    }
    

    float diff_alpha = abs(alpha - alpha_prec);
    float diff_alpha2 = abs(alpha2 - alpha_prec);

    


    if (diff_alpha > 2 * pi) {
        diff_alpha -= 2 * pi;//On reste entre 0 et 2*pi
    }
    if (diff_alpha < 0) {
        diff_alpha += 2 * pi;
    }


    if (diff_alpha2 > 2 * pi) {
        diff_alpha2 -= 2 * pi;//On reste entre 0 et 2*pi
    }
    if (diff_alpha2 < 0) {
        diff_alpha2 += 2 * pi;
    }

    
    std::cout << "\n\nFound alpha are " << alpha / (2 * pi) * 360 <<" and "<< alpha2 / (2 * pi) * 360 <<
        "(Radians values are "<<alpha<<" and "<< alpha2 << ")\n"<< std::endl;

    std::cout << "The differences are " << diff_alpha << " and " << diff_alpha2 << std::endl;
    


    bool Isgreater = (diff_alpha > diff_alpha2);
    if (Isgreater) {
        alpha = alpha2;
    }
        
    
    /*alpha += alpha_prec; //on compense la soustraction
    if (alpha > 3 / 2 * pi) {
        alpha -= 2 * pi;//On reste entre -pi/2 et 3pi/2
    }
    if (alpha < -1 / 2 * pi) {
        alpha+= 2 * pi;
    }//réécriture dans le bon intervalle*/

    return alpha;
}


float exact_answer(float x, float z, float& alpha_prec) {
   
    float X = (arm * arm - forearm * forearm + x * x + z * z) / (2 * arm);
    float length = sqrt(x * x + z * z);

    float phi = asin(x / length);
    float phi2 = acos(z / length);

    float blub = asin(X / length);

    std::cout << "Phi vaut " << phi << " et l'autre terme vaut " << blub << std::endl;
    std::cout << "Phi2 vaut " << phi2 << std::endl; 

    float alpha = blub - phi;



    
    if (alpha > pi) {
        alpha -= 2 * pi;//On reste entre -pi et pi
    }
    if (alpha < -pi) {
        alpha += 2 * pi;
    }
    

    //Toute la suite est potentiellement non nécessaire. 
    //Pourrait être remplacé en prenant le plus petit des deux : pour z positif, les angles sont égaux, mais pour z négatif l'un diminue plus que l'autre et permet de passer en z négatif. 

    float alpha2 = blub - phi2; //l'autre solution

    /*
    if (alpha2 > pi) {
        alpha2 -= 2 * pi;//On reste entre -pi et pi
    }
    if (alpha2 < -pi) {
        alpha2 += 2 * pi;
    }


    float diff_alpha = abs(alpha - alpha_prec);
    float diff_alpha2 = abs(alpha2 - alpha_prec);




    if (diff_alpha > 2 * pi) {
        diff_alpha -= 2 * pi;//On reste entre 0 et 2*pi
    }
    if (diff_alpha < 0) {
        diff_alpha += 2 * pi;
    }


    if (diff_alpha2 > 2 * pi) {
        diff_alpha2 -= 2 * pi;//On reste entre 0 et 2*pi
    }
    if (diff_alpha2 < 0) {
        diff_alpha2 += 2 * pi;
    }


    std::cout << "\nFound alpha are " << alpha / (2 * pi) * 360 << " and " << alpha2 / (2 * pi) * 360 <<
        "(Radians values are " << alpha << " and " << alpha2 << ")\n" << std::endl;

    std::cout << "The differences are " << diff_alpha << " and " << diff_alpha2 << std::endl;



    bool Isgreater = (diff_alpha > diff_alpha2);
    if (Isgreater) {
        alpha = alpha2;
    }


    /*alpha += alpha_prec; //on compense la soustraction
    if (alpha > 3 / 2 * pi) {
        alpha -= 2 * pi;//On reste entre -pi/2 et 3pi/2
    }
    if (alpha < -1 / 2 * pi) {
        alpha+= 2 * pi;
    }//réécriture dans le bon intervalle/*

    */

    alpha = std::min(alpha, alpha2);
    
    
    return alpha2;

}; //precision, x et y et alpha_précédent



int main() {

    
    int stop = 0;

    bool IsReset = false;
    bool click_mode = false; //Mode clic désactivé par défaut

    float alpha_prec = -pi/2+0.1; //le alpha initial est très proche de -pi/2 

    float x = X_INIT;
    float y = Y_INIT;
    float z = Z_INIT; //les trois positions cardinales qui seront utilisées pour le robot.
    //Pour les tests, x, y et z iront de 0 à 999, à la première décimale

    std::cout << "Press p to exit. Recognized inputs are z, q, s, d, and spacebar." 
        <<"Press r to reset the system to it's original position." 
        <<"\nDisplayed values in centimeters for distances and radians for angles."<< std::endl;

    std::cout << "True position values : " << x << " " << y << " " << z << std::endl;
    Input_Processing(0, 0, 0, x, y, z, alpha_prec);
    std::cout << "\r" << x << " " << y << " " << z;


    char coarse;

    while (stop == 0) {



        if (!IsReset) coarse = getch(); //Récupérer un caractère unique de la console
        int input = Test_Input_Recognition(coarse);// traduire le caractère en digit associé à un input
        
        //std::cout << "\nL'input est : " << input << std::endl;

        if (input == end) { //Comande d'arrêt
            std::cout << "\nEnd of Programm" << std::endl;
            return 0;
        }
        else if(input == unknown){ //commande inconnue
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

            std::cout << "\r" << std::setprecision(3)  << x << " " 
                << std::setprecision(3)  <<  y <<" "
                << std::setprecision(3) /*<< std::setw(4)*/ <<  z ;
            //Ne pas mettre de \n, sinon le saut de ligne annule en partie le \r
            //Traiter les inputs ici
            IsReset = false; //Interruption du processus de reset si une commande est donnée.
        }

        
        std::cout << "\r" << x << " " << y << " " << z ; //Attention à la taille des chiffres affichés ! elle doit être constante
        //\r sert à revenir au début de la dernière ligne et ainsi ne pas print 500 000 lignes
        
        _sleep(frame); // petit délai pour ne pas surcharger les inputs. 

    }

    

}