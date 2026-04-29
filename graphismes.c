#include <stdio.h>
#include <string.h>
#include "graphismes.h"

/*
    Charge une image.

    Si l’image n’existe pas :
    - on affiche un message dans la console
    - on retourne NULL

    Le NULL n’est pas grave :
    les fonctions de dessin utiliseront un fallback
    avec des formes simples.
*/
static BITMAP *charger_image(const char *chemin) {
    BITMAP *img = load_bitmap(chemin, NULL);

    if (img == NULL) {
        printf("Image manquante : %s\n", chemin);
    }

    return img;
}

/*
    Charge toutes les ressources graphiques.

    Important :
    memset met tous les pointeurs à NULL au départ.
    Comme ça, si une image ne charge pas, on sait qu’elle vaut NULL.
*/
void charger_ressources_graphiques(RessourcesGraphiques *res) {
    if (res == NULL) return;

    memset(res, 0, sizeof(RessourcesGraphiques));

    res->joueur = charger_image("assets/sprites/joueur.bmp");
    res->bulle = charger_image("assets/sprites/bulle.bmp");
    res->slime = charger_image("assets/sprites/slime.bmp");
    res->boule_feu = charger_image("assets/sprites/boule_feu.bmp");
    res->boss = charger_image("assets/sprites/boss.bmp");

    res->decor1 = charger_image("assets/decors/decor1.bmp");
    res->decor2 = charger_image("assets/decors/decor2.bmp");
    res->decor3 = charger_image("assets/decors/decor3.bmp");
    res->decor4 = charger_image("assets/decors/decor4.bmp");

    /*
        PLUS TARD POUR LES ANIMATIONS :

        Exemple :
        res->joueur_marche[0] = charger_image("assets/sprites/joueur_marche_0.bmp");
        res->joueur_marche[1] = charger_image("assets/sprites/joueur_marche_1.bmp");
        res->joueur_marche[2] = charger_image("assets/sprites/joueur_marche_2.bmp");
        res->joueur_marche[3] = charger_image("assets/sprites/joueur_marche_3.bmp");

        Mais pour l’instant, on garde une base simple.
    */
}

/*
    Libère toutes les images chargées.

    On remet chaque pointeur à NULL après destruction
    pour éviter les bugs de double destruction.
*/
void liberer_ressources_graphiques(RessourcesGraphiques *res) {
    if (res == NULL) return;

    if (res->joueur != NULL) {
        destroy_bitmap(res->joueur);
        res->joueur = NULL;
    }

    if (res->bulle != NULL) {
        destroy_bitmap(res->bulle);
        res->bulle = NULL;
    }

    if (res->slime != NULL) {
        destroy_bitmap(res->slime);
        res->slime = NULL;
    }

    if (res->boule_feu != NULL) {
        destroy_bitmap(res->boule_feu);
        res->boule_feu = NULL;
    }

    if (res->boss != NULL) {
        destroy_bitmap(res->boss);
        res->boss = NULL;
    }

    if (res->decor1 != NULL) {
        destroy_bitmap(res->decor1);
        res->decor1 = NULL;
    }

    if (res->decor2 != NULL) {
        destroy_bitmap(res->decor2);
        res->decor2 = NULL;
    }

    if (res->decor3 != NULL) {
        destroy_bitmap(res->decor3);
        res->decor3 = NULL;
    }

    if (res->decor4 != NULL) {
        destroy_bitmap(res->decor4);
        res->decor4 = NULL;
    }

    /*
        PLUS TARD POUR LES ANIMATIONS :

        Si tu ajoutes plusieurs images par animation,
        il faudra aussi les détruire ici.

        Exemple :
        for (i = 0; i < 4; i++) {
            if (res->joueur_marche[i] != NULL) {
                destroy_bitmap(res->joueur_marche[i]);
                res->joueur_marche[i] = NULL;
            }
        }
    */
}

/*
    Renvoie le décor correspondant au numéro du niveau.
*/
BITMAP *obtenir_decor_niveau(RessourcesGraphiques *res, int numero_niveau) {
    if (res == NULL) return NULL;

    switch (numero_niveau) {
        case 1:
            return res->decor1;

        case 2:
            return res->decor2;

        case 3:
            return res->decor3;

        case 4:
            return res->decor4;

        default:
            return res->decor1;
    }
}

/*
    Dessine le décor du niveau.

    Si l’image du décor existe :
    - on l’étire à la taille de l’écran

    Si elle n’existe pas :
    - on affiche un fond simple
*/
void dessiner_decor(BITMAP *buffer, RessourcesGraphiques *res, int numero_niveau) {
    BITMAP *decor;

    if (buffer == NULL) return;

    decor = obtenir_decor_niveau(res, numero_niveau);

    if (decor != NULL) {
        stretch_blit(decor,
                     buffer,
                     0, 0,
                     decor->w, decor->h,
                     0, 0,
                     SCREEN_W, SCREEN_H);
    } else {
        clear_to_color(buffer, makecol(20, 20, 40));
    }
}

/*
    Dessine le joueur.

    Pour l’instant :
    - image simple si elle existe
    - rectangle bleu sinon

    Plus tard :
    - remplacer par dessiner_joueur_anime(...)
*/
void dessiner_joueur(BITMAP *buffer, RessourcesGraphiques *res, int x, int y) {
    if (buffer == NULL) return;

    if (res != NULL && res->joueur != NULL) {
        draw_sprite(buffer, res->joueur, x, y);
    } else {
        rectfill(buffer,
                 x,
                 y,
                 x + 40,
                 y + 50,
                 makecol(0, 120, 255));
    }
}

/*
    Dessine une bulle.

    x, y représentent le centre de la bulle.
*/
void dessiner_bulle(BITMAP *buffer,
                    RessourcesGraphiques *res,
                    int x,
                    int y,
                    int rayon) {
    if (buffer == NULL) return;

    if (res != NULL && res->bulle != NULL) {
        stretch_sprite(buffer,
                       res->bulle,
                       x - rayon,
                       y - rayon,
                       rayon * 2,
                       rayon * 2);
    } else {
        circlefill(buffer,
                   x,
                   y,
                   rayon,
                   makecol(255, 0, 0));
    }
}

/*
    Dessine un slime.

    x, y représentent le centre du slime.
*/
void dessiner_slime(BITMAP *buffer,
                    RessourcesGraphiques *res,
                    int x,
                    int y,
                    int rayon) {
    if (buffer == NULL) return;

    if (res != NULL && res->slime != NULL) {
        stretch_sprite(buffer,
                       res->slime,
                       x - rayon,
                       y - rayon,
                       rayon * 2,
                       rayon * 2);
    } else {
        circlefill(buffer,
                   x,
                   y,
                   rayon,
                   makecol(0, 200, 80));
    }
}

/*
    Dessine une boule de feu.

    Les boules de feu ne sont pas dans niveaux.c avec des coordonnées fixes.
    Elles seront créées plus tard avec un spawn aléatoire.
*/
void dessiner_boule_feu(BITMAP *buffer,
                        RessourcesGraphiques *res,
                        int x,
                        int y,
                        int rayon) {
    if (buffer == NULL) return;

    if (res != NULL && res->boule_feu != NULL) {
        stretch_sprite(buffer,
                       res->boule_feu,
                       x - rayon,
                       y - rayon,
                       rayon * 2,
                       rayon * 2);
    } else {
        circlefill(buffer,
                   x,
                   y,
                   rayon,
                   makecol(255, 120, 0));
    }
}

/*
    Dessine le boss.

    Le comportement du boss reste dans boss.c.
    Ici on ne fait que l’affichage.
*/
void dessiner_boss(BITMAP *buffer, RessourcesGraphiques *res, Boss *boss) {
    if (buffer == NULL || boss == NULL) return;

    if (boss->actif == 0) return;

    if (res != NULL && res->boss != NULL) {
        stretch_sprite(buffer,
                       res->boss,
                       boss->x,
                       boss->y,
                       boss->largeur,
                       boss->hauteur);
    } else {
        rectfill(buffer,
                 boss->x,
                 boss->y,
                 boss->x + boss->largeur,
                 boss->y + boss->hauteur,
                 makecol(150, 0, 200));
    }
}

/*
    Affiche un niveau complet.

    Cette fonction est appelée depuis draw_jeu() dans jeu.c.

    Important :
    - main.c ne dessine rien
    - jeu.c appelle cette fonction
    - graphismes.c gère l’affichage

    Le paramètre frame est déjà prévu pour les animations futures.
*/
void afficher_niveau_graphique(BITMAP *buffer,
                               RessourcesGraphiques *res,
                               NiveauConfig *niveau,
                               Boss *boss,
                               int frame) {
    int i;

    if (buffer == NULL || res == NULL || niveau == NULL) return;

    /*
        1. Décor du niveau
    */
    dessiner_decor(buffer, res, niveau->numero);

    /*
        2. Bulles initiales

        Pour l’instant, on dessine les bulles du tableau initial.
        Plus tard, quand les bulles se divisent, il faudra afficher
        la liste chaînée dynamique des bulles au lieu de ce tableau.
    */
    for (i = 0; i < niveau->nb_bulles_initiales; i++) {
        dessiner_bulle(buffer,
                       res,
                       (int)niveau->bulles[i].x,
                       (int)niveau->bulles[i].y,
                       (int)niveau->bulles[i].rayon);

        /*
            PLUS TARD POUR ANIMATION BULLE :

            dessiner_bulle_animee(buffer,
                                  res,
                                  niveau->bulles[i].x,
                                  niveau->bulles[i].y,
                                  niveau->bulles[i].rayon,
                                  frame);
        */
    }

    /*
        3. Slimes initiaux

        Comme pour les bulles, plus tard les slimes pourront être
        dans une liste dynamique si tu veux les faire bouger/supprimer.
    */
    for (i = 0; i < niveau->nb_slimes_initiales; i++) {
        dessiner_slime(buffer,
                       res,
                       (int)niveau->slimes[i].x,
                       (int)niveau->slimes[i].y,
                       (int)niveau->slimes[i].rayon);

        /*
            PLUS TARD POUR ANIMATION SLIME :

            dessiner_slime_anime(buffer,
                                 res,
                                 niveau->slimes[i].x,
                                 niveau->slimes[i].y,
                                 niveau->slimes[i].rayon,
                                 frame);
        */
    }

    /*
        4. Boules de feu

        On ne les affiche pas ici pour l’instant,
        car elles ne sont PAS présentes au début du niveau.

        Le niveau contient seulement :
        niveau->boule_feu.active
        niveau->boule_feu.delai_min_spawn
        niveau->boule_feu.delai_max_spawn
        niveau->boule_feu.vitesse_y
        niveau->boule_feu.rayon

        Plus tard, quand tu auras une liste de boules de feu actives,
        tu feras par exemple :

        dessiner_liste_boules_feu(buffer, res, &liste_boules_feu, frame);
    */

    /*
        5. Boss

        Présent seulement au niveau 4.
    */
    if (niveau->boss_present && boss != NULL) {
        dessiner_boss(buffer, res, boss);

        /*
            PLUS TARD POUR ANIMATION BOSS :

            dessiner_boss_anime(buffer, res, boss, frame);

            PLUS TARD POUR BARRE DE VIE :

            afficher_barre_vie_boss(buffer, boss);
        */
    }

    /*
        6. HUD simple

        Plus tard :
        - score
        - vies
        - timer réel
        - arme active
    */
    textprintf_ex(buffer,
                  font,
                  10,
                  10,
                  makecol(255, 255, 255),
                  -1,
                  "%s | Timer : %d | Frame : %d",
                  niveau->nom,
                  niveau->temps_limite,
                  frame);
}