/* =========================================================
*  main.c  —  Point d'entrée unique
 * ========================================================= */

#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "graphismes.h"
#include "menu.h"

int main(void)
{
    Ressources res;
    BITMAP    *buffer;

    /* ── 1. Init Allegro de base ──────────────────────────── */
    allegro_init();
    install_keyboard();
    install_mouse();

    /* ── 2. Mode vidéo AVANT tout chargement d'image ─────── */
    set_color_depth(24);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,
                     WINDOW_W, WINDOW_H, 0, 0) != 0) {
        allegro_message("Erreur fenetre : %s", allegro_error);
        return EXIT_FAILURE;
                     }
    set_window_title("Super Bulles");

    /* ── 3. Chargement ressources APRES set_gfx_mode ─────── */
    fprintf(stderr, "[INFO] Chargement assets...\n");

    if (!graphique_charger_ressources(&res,
            "C:/informatique ing 1v2/projet2/superpong"
            "/cmake-build-debug/assets")) {
        allegro_message("Erreur chargement ressources.");
        return EXIT_FAILURE;
            }

    /* ── 4. Buffer ───────────────────────────────────────── */
    buffer = create_bitmap(WINDOW_W, WINDOW_H);
    if (!buffer) {
        allegro_message("Impossible de creer le buffer.");
        graphique_liberer_ressources(&res);
        return EXIT_FAILURE;
    }

    /* ── 5. Boucle principale ────────────────────────────── */
    menu(buffer, &res);

    destroy_bitmap(buffer);
    graphique_liberer_ressources(&res);
    return EXIT_SUCCESS;
}
END_OF_MAIN()