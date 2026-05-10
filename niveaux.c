#include <stdio.h>
#include <string.h>
#include "niveaux.h"

/* =========================================================
   NIVEAUX.C — Configuration des niveaux
   Oni no Michi · Allegro 4.4
   ========================================================= */

#define LARGEUR_DEFAUT  800
#define HAUTEUR_DEFAUT  600
#define SOL_DEFAUT      540   /* y du sol en pixels */

void charger_niveau(NiveauConfig *niveau, int numero)
{
    if (niveau == NULL) return;

    memset(niveau, 0, sizeof(NiveauConfig));

    niveau->numero  = numero;
    niveau->largeur = LARGEUR_DEFAUT;
    niveau->hauteur = HAUTEUR_DEFAUT;

    switch (numero) {

        case 1:
            strcpy(niveau->nom,   "Niveau 1 - Initiation");
            strcpy(niveau->decor, "assets/decors/decor1.bmp");

            niveau->temps_limite = 90;
            niveau->objectif     = OBJ_ELIMINER_BULLES;

            niveau->spawn_x = 100.0f;
            niveau->spawn_y = (float)(SOL_DEFAUT - 28);

            niveau->nb_bulles_initiales = 2;
            niveau->bulles[0] = (BulleInit){200, 120,  120, -250, 32, 2, 0};
            niveau->bulles[1] = (BulleInit){600, 120, -120, -250, 32, 2, 0};

            niveau->nb_slimes_initiales = 1;
            niveau->slimes[0] = (SlimeInit){400, 480, 80, 0, 22, 1};

            niveau->boule_feu.active = 0;
            break;

        case 2:
            strcpy(niveau->nom,   "Niveau 2 - Acceleration");
            strcpy(niveau->decor, "assets/decors/decor2.bmp");

            niveau->temps_limite = 85;
            niveau->objectif     = OBJ_ELIMINER_BULLES;

            niveau->spawn_x = 100.0f;
            niveau->spawn_y = (float)(SOL_DEFAUT - 28);

            niveau->nb_bulles_initiales = 3;
            niveau->bulles[0] = (BulleInit){180, 110,  150, -260, 36, 3, 0};
            niveau->bulles[1] = (BulleInit){420, 100, -130, -240, 32, 2, 0};
            niveau->bulles[2] = (BulleInit){650, 130, -160, -250, 28, 2, 0};

            niveau->nb_slimes_initiales = 2;
            niveau->slimes[0] = (SlimeInit){250, 480,  90, 0, 22, 1};
            niveau->slimes[1] = (SlimeInit){550, 480, -90, 0, 22, 1};

            niveau->boule_feu.active         = 1;
            niveau->boule_feu.delai_min_spawn = 3;
            niveau->boule_feu.delai_max_spawn = 6;
            niveau->boule_feu.vitesse_y       = 180;
            niveau->boule_feu.rayon           = 12;
            break;

        case 3:
            strcpy(niveau->nom,   "Niveau 3 - Eclairs et feu");
            strcpy(niveau->decor, "assets/decors/decor3.bmp");

            niveau->temps_limite = 80;
            niveau->objectif     = OBJ_ELIMINER_BULLES;

            niveau->spawn_x = 100.0f;
            niveau->spawn_y = (float)(SOL_DEFAUT - 28);

            niveau->nb_bulles_initiales = 4;
            niveau->bulles[0] = (BulleInit){150, 100,  170, -270, 36, 3, 1};
            niveau->bulles[1] = (BulleInit){350, 130, -160, -260, 32, 2, 0};
            niveau->bulles[2] = (BulleInit){550, 120,  180, -280, 32, 2, 1};
            niveau->bulles[3] = (BulleInit){700, 100, -150, -250, 28, 2, 0};

            niveau->nb_slimes_initiales = 3;
            niveau->slimes[0] = (SlimeInit){180, 480,  100, 0, 24, 2};
            niveau->slimes[1] = (SlimeInit){400, 480, -100, 0, 24, 2};
            niveau->slimes[2] = (SlimeInit){620, 480,  120, 0, 22, 1};

            niveau->boule_feu.active         = 1;
            niveau->boule_feu.delai_min_spawn = 2;
            niveau->boule_feu.delai_max_spawn = 4;
            niveau->boule_feu.vitesse_y       = 230;
            niveau->boule_feu.rayon           = 13;
            break;

        case 4:
            strcpy(niveau->nom,   "Niveau 4 - Boss final");
            strcpy(niveau->decor, "assets/decors/decor4.bmp");

            niveau->temps_limite = 120;
            niveau->objectif     = OBJ_ELIMINER_BULLES_ET_BOSS;

            niveau->spawn_x = 100.0f;
            niveau->spawn_y = (float)(SOL_DEFAUT - 28);

            niveau->boss_present  = 1;
            niveau->pv_boss       = 10;
            niveau->boss_spawn_x  = 600.0f;
            niveau->boss_spawn_y  = (float)(SOL_DEFAUT - 128);

            niveau->nb_bulles_initiales = 3;
            niveau->bulles[0] = (BulleInit){200, 110,  170, -270, 34, 3, 1};
            niveau->bulles[1] = (BulleInit){500, 100, -170, -260, 34, 3, 1};
            niveau->bulles[2] = (BulleInit){650, 130,  150, -250, 28, 2, 0};

            niveau->nb_slimes_initiales = 0;
            niveau->boule_feu.active    = 0;
            break;

        default:
            charger_niveau(niveau, 1);
            break;
    }
}

/* =========================================================
   COLLISION TUILES
   Sol simple à hauteur fixe — à enrichir avec une vraie
   tilemap quand le système de carte sera en place.
   ========================================================= */

int niveau_trouver_sol(NiveauConfig *n, int x, int bas, int vy)
{
    (void)x; (void)vy;
    if (bas >= SOL_DEFAUT) return SOL_DEFAUT;
    return -1;   /* pas de sol trouvé */
}

int niveau_trouver_plafond(NiveauConfig *n, int x, int haut, int vy)
{
    (void)x; (void)vy;
    if (haut <= 0) return 0;
    return -1;   /* pas de plafond trouvé */
}

int niveau_est_solide(NiveauConfig *n, int x, int y)
{
    (void)n;
    /* bords de l'écran et sol */
    if (x < 0 || x >= LARGEUR_DEFAUT) return 1;
    if (y >= SOL_DEFAUT)              return 1;
    return 0;
}