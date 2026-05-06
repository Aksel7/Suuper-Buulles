/**************************************************************************
SUPER BULLES - Prototypage
- Déplacements horizontaux fluides
- Mécanique de saut avec gravité (Style Plateforme)
- Contraintes d'écran strictes
**************************************************************************/

#include <allegro.h>
#include <stdio.h>
#include <stdbool.h>

// --- CONSTANTES PHYSIQUES ---
#define GRAVITY 1.0f       // Force d'attraction vers le bas
#define JUMP_FORCE 16.0f   // Puissance de l'impulsion du saut

// --- STRUCTURES ---
typedef struct {
    int x, y;
    int taille;
    int speed_x;          // Vitesse de déplacement horizontal
    float vy;             // Vitesse verticale (flottant pour une courbe de saut fluide)
    bool is_jumping;      // Pour empêcher le double saut en l'air
    int color;
} Player;

typedef struct  {
    int x, y;
    int taille;
    int speed_x;
    float vy;
    int color;
} Bulles;

typedef struct  {
    int x, y;
    int taille;
    int speed_x;
    float vy;
    int color;
} Projectiles;

// --- VARIABLES GLOBALES ---

BITMAP *page;
Player joueur;
int tempoglobale = 15;

// --- PROTOTYPES ---
void initialisation_allegro();
void init_game();
void update_game();
void render_game();
void cleanup_game();

// --- PROGRAMME PRINCIPAL ---
int main()
{
    initialisation_allegro();
    init_game();

    while (!key[KEY_ESC])
    {
        update_game();
        render_game();
        rest(tempoglobale);
    }

    cleanup_game();
    return 0;
}
END_OF_MAIN();

// --- DEFINITIONS DES FONCTIONS ---

void init_game() {
    page = create_bitmap(SCREEN_W, SCREEN_H);

    joueur.taille = 40;
    joueur.x = SCREEN_W / 2;
    joueur.y = SCREEN_H - joueur.taille; // On commence posé sur le sol
    joueur.speed_x = 6;
    joueur.vy = 0;
    joueur.is_jumping = false;
    joueur.color = makecol(0, 0, 255); // Carré bleu
}

void update_game() {
    // 1. Déplacements horizontaux (Gauche / Droite)
    if (key[KEY_LEFT]) {
        joueur.x -= joueur.speed_x;
    }
    if (key[KEY_RIGHT]) {
        joueur.x += joueur.speed_x;
    }

    // 2. Déclenchement du saut (Flèche HAUT)
    // On ne peut sauter QUE si on n'est pas déjà en train de sauter
    if (key[KEY_UP] && !joueur.is_jumping) {
        joueur.vy = -JUMP_FORCE; // Impulsion vers le haut (l'axe Y va vers le bas à l'écran)
        joueur.is_jumping = true;
    }

    // 3. Application de la gravité et mise à jour de la position verticale
    joueur.vy += GRAVITY;       // La gravité tire constamment le joueur vers le bas
    joueur.y += (int)joueur.vy; // On applique la vitesse verticale à la position

    // 4. Contraintes aux limites (Mur gauche et droite)
    if (joueur.x < 0) {
        joueur.x = 0;
    } else if (joueur.x + joueur.taille > SCREEN_W) {
        joueur.x = SCREEN_W - joueur.taille;
    }

    // 5. Gestion des collisions avec le sol et le plafond
    // Le sol est tout en bas de l'écran (SCREEN_H)
    if (joueur.y + joueur.taille >= SCREEN_H) {
        joueur.y = SCREEN_H - joueur.taille; // Recale le joueur au niveau exact du sol
        joueur.vy = 0;                       // Annule la vitesse de chute
        joueur.is_jumping = false;           // Le joueur est de nouveau autorisé à sauter
    }
    // Blocage au plafond (optionnel, pour éviter qu'il sorte par le haut)
    else if (joueur.y < 0) {
        joueur.y = 0;
        joueur.vy = 0; // S'il cogne le plafond, il commence à retomber immédiatement
    }
}

void render_game() {
    clear_to_color(page, makecol(255, 255, 255)); // Fond blanc

    // Dessin du joueur
    rectfill(page, joueur.x, joueur.y, joueur.x + joueur.taille, joueur.y + joueur.taille, joueur.color);

    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void cleanup_game() {
    if (page != NULL) destroy_bitmap(page);
}

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) {
        allegro_message("Problème mode graphique : %s", allegro_error);
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}