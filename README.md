# Cybathlon-2020



Ce Dossier contient le code actuel du Projet Cybathlon 2020 de l'ENSTA Paris.

Veuillez utiliser le makefile pour compiler. Une édition pourra être nécessaire pour trouver les fichiers nécessaires à la compilation du programme. Le programme nécessite la librairie SDL2.

Le programme affiche une simulation de bras robot et se pilote avec un joystick et 2 boutons.
Le premier bouton sert à changer de mode par une pression simple, et à quitter le programme par une pression prolongée.
Le second bouton sert à réinitialiser la position du robot. 
Le joystick permet de piloter le robot d'avant en arrière et de droite à gauche. Le pilotage de droite à gauche n'est pas affiché par le programme et n'est p'as réalisé en translation mais en rotation autour de l'axe central.
Pour piloter le bras de haut en bas, appuyer sur le premier bouton. Cela change le mode de pilotage du robot et permet de le contrôler de haut en bas et de gauche à droite. 
Ce bouton est par défaut, sur ma manette, le bouton du bas de la face droite de la manette. (A pour une Xbox, B pour une Nintendo Switch, et X pour une PS4)

Pour réinitialiser la position du robot, appuyer sur le bouton droit de la face droite, soit B pour une Xbox, A pour une Switch ou Rond pour une PS4.

La réinitialisation a lieu en direct et le robot calcule bien chaque étape du déplacement nécessaire à sa réinitialisation. il ne s'agit pas d'une simple animation mais d'une réelle réinitialisation du programme dans sa position initiale. 
