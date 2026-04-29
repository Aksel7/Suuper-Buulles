#include <allegro.h>
#include "jeu.h"

int main(void) {
    Jeu jeu;

    if (!initialiser_jeu(&jeu)) {
        return 1;
    }

    boucle_jeu(&jeu);

    liberer_jeu(&jeu);

    return 0;
}
END_OF_MAIN();