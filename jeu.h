#ifndef JEU_H
#define JEU_H

#include <allegro.h>
#include "types.h"

/* =========================================================
   JEU.H
   jeu() : boucle de jeu complète.
   Appelle logique.c (joueur/bulle/boss/niveau)
   et graphique.c pour l'affichage.
   Retourne quand le joueur revient au menu ou quitte.
   ========================================================= */

void jeu(BITMAP *buffer, Ressources *res,
         const char *pseudo, int niveau_depart,
         int nb_joueurs, int vies_depart, int score_depart);

#endif /* JEU_H */