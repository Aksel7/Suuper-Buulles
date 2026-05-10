#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include "types.h"

/* =========================================================
   MENU.H
   menu() : navigation complète, appelle jeu() quand une
   partie est lancée. Retourne quand le joueur quitte.
   ========================================================= */

void menu(BITMAP *buffer, Ressources *res);

#endif /* MENU_H */