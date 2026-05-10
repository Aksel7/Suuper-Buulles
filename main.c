/* =========================================================
*  main.c  —  Point d'entrée unique
 *  Init Allegro → appel menu()
 *  Tout le reste est dans menu.c / jeu.c / graphique.c
 * ========================================================= */

#include <allegro.h>
#include <stdlib.h>
#include "types.h"
#include "graphismes.h"
#include "menu.h"

int main(void)
{
    Ressources res;
    BITMAP    *buffer;

    graphique_init_allegro();

    if (!graphique_charger_ressources(&res, "assets")) {
        allegro_exit();
        return EXIT_FAILURE;
    }

    buffer = create_bitmap(WINDOW_W, WINDOW_H);
    if (!buffer) {
        allegro_message("Impossible de creer le buffer.");
        graphique_liberer_ressources(&res);
        return EXIT_FAILURE;
    }

    menu(buffer, &res);   /* tout le programme est ici */

    destroy_bitmap(buffer);
    graphique_liberer_ressources(&res);
    return EXIT_SUCCESS;
}
END_OF_MAIN()