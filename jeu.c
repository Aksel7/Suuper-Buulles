#include <string.h>
#include <allegro.h>
#include "jeu.h"

static int initialiser_allegro(void) {
    allegro_init();
    install_keyboard();
    install_timer();

    set_color_depth(32);

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,
                     LARGEUR_ECRAN,
                     HAUTEUR_ECRAN,
                     0,
                     0) != 0) {
        allegro_message("Erreur mode graphique : %s", allegro_error);
        return 0;
    }

    return 1;
}

int initialiser_jeu(Jeu *jeu) {
    if (jeu == NULL) return 0;

    memset(jeu, 0, sizeof(Jeu));

    if (!initialiser_allegro()) {
        return 0;
    }

    jeu->etat = ETAT_JEU;
    jeu->niveau_courant = 1;
    jeu->frame = 0;

    jeu->buffer = create_bitmap(SCREEN_W, SCREEN_H);

    if (jeu->buffer == NULL) {
        allegro_message("Erreur creation buffer");
        return 0;
    }

    charger_ressources_graphiques(&jeu->ressources);

    charger_niveau(&jeu->niveau, jeu->niveau_courant);

    if (jeu->niveau.boss_present) {
        init_boss(&jeu->boss, jeu->niveau.pv_boss);
    }

    return 1;
}

void boucle_jeu(Jeu *jeu) {
    while (jeu->etat != ETAT_QUITTER) {
        update_jeu(jeu);
        draw_jeu(jeu);

        rest(1000 / FPS);
    }
}

void update_jeu(Jeu *jeu) {
    if (jeu == NULL) return;

    if (key[KEY_ESC]) {
        jeu->etat = ETAT_QUITTER;
        return;
    }

    /*
        Compteur utilisé plus tard par les animations.
        Le main ne gère pas les animations.
    */
    jeu->frame++;

    /*
        Plus tard ici :
        update_joueur();
        update_bulles();
        update_slimes();
        update_boules_feu();
        update_projectiles();
        update_collisions();
        update_timer();
    */

    if (jeu->niveau.boss_present) {
        update_boss(&jeu->boss);
    }
}

void draw_jeu(Jeu *jeu) {
    if (jeu == NULL || jeu->buffer == NULL) return;

    clear_bitmap(jeu->buffer);

    /*
        Tout l'affichage passe par graphismes.c.
        Les animations seront ajoutées dans graphismes.c,
        pas dans main.c.
    */
    afficher_niveau_graphique(jeu->buffer,
                              &jeu->ressources,
                              &jeu->niveau,
                              &jeu->boss,
                              jeu->frame);

    blit(jeu->buffer,
         screen,
         0,
         0,
         0,
         0,
         SCREEN_W,
         SCREEN_H);
}

void liberer_jeu(Jeu *jeu) {
    if (jeu == NULL) return;

    liberer_ressources_graphiques(&jeu->ressources);

    if (jeu->buffer != NULL) {
        destroy_bitmap(jeu->buffer);
        jeu->buffer = NULL;
    }
}