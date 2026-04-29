#ifndef GRAPHISMES_H
#define GRAPHISMES_H

#include <allegro.h>
#include "niveaux.h"
#include "boss.h"

/*
    Structure qui contient toutes les images du jeu.

    Pour l’instant :
    - 1 image par entité
    - pas encore d’animation

    Plus tard, pour les animations, on pourra remplacer :
    BITMAP *joueur;
    par exemple par :
    BITMAP *joueur_marche[4];
*/
typedef struct {
    BITMAP *joueur;
    BITMAP *bulle;
    BITMAP *slime;
    BITMAP *boule_feu;
    BITMAP *boss;

    BITMAP *decor1;
    BITMAP *decor2;
    BITMAP *decor3;
    BITMAP *decor4;
} RessourcesGraphiques;

/* Chargement / libération */
void charger_ressources_graphiques(RessourcesGraphiques *res);
void liberer_ressources_graphiques(RessourcesGraphiques *res);

/* Décor */
BITMAP *obtenir_decor_niveau(RessourcesGraphiques *res, int numero_niveau);
void dessiner_decor(BITMAP *buffer, RessourcesGraphiques *res, int numero_niveau);

/* Entités simples */
void dessiner_joueur(BITMAP *buffer, RessourcesGraphiques *res, int x, int y);
void dessiner_bulle(BITMAP *buffer, RessourcesGraphiques *res, int x, int y, int rayon);
void dessiner_slime(BITMAP *buffer, RessourcesGraphiques *res, int x, int y, int rayon);
void dessiner_boule_feu(BITMAP *buffer, RessourcesGraphiques *res, int x, int y, int rayon);
void dessiner_boss(BITMAP *buffer, RessourcesGraphiques *res, Boss *boss);

/*
    Fonction globale d’affichage d’un niveau.

    Elle est appelée par jeu.c dans draw_jeu().
    Plus tard, les animations seront ajoutées ici,
    ou dans des fonctions appelées depuis ici.
*/
void afficher_niveau_graphique(BITMAP *buffer,
                               RessourcesGraphiques *res,
                               NiveauConfig *niveau,
                               Boss *boss,
                               int frame);

#endif