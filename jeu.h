#ifndef JEU_H
#define JEU_H

#include <allegro.h>
#include "niveaux.h"
#include "graphismes.h"
#include "boss.h"

#define LARGEUR_ECRAN 800
#define HAUTEUR_ECRAN 600
#define FPS 60

typedef enum {
    ETAT_MENU,
    ETAT_JEU,
    ETAT_QUITTER
} EtatProgramme;

typedef struct {
    EtatProgramme etat;

    BITMAP *buffer;

    RessourcesGraphiques ressources;
    NiveauConfig niveau;
    Boss boss;

    int niveau_courant;
    int frame;
} Jeu;

int initialiser_jeu(Jeu *jeu);
void boucle_jeu(Jeu *jeu);
void update_jeu(Jeu *jeu);
void draw_jeu(Jeu *jeu);
void liberer_jeu(Jeu *jeu);

#endif