/**************************************************************************
 SUPER BULLES - Prototype Complet ING1

 **************************************************************************/

#include <allegro.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h> // Nécessaire pour sqrt et pow (collisions)

// --- CONSTANTES PHYSIQUES ET LIMITES ---
#define GRAVITY 1.0f
#define BULLE_GRAVITY 0.2f   // Gravité plus faible pour l'effet "flottant" des bulles
#define JUMP_FORCE 25.0f
#define MAX_PROJECTILES 10
#define PROJECTILE_SPEED 12
#define RELOAD_TIME 15
#define MAX_PLATEFORMES 6
#define MAX_BULLES 20

// --- STRUCTURES ---
typedef struct {
    float x, y;
    int taille;
    int speed_x;
    float vy;
    bool is_jumping;
    int color;
    int reload_timer;
} Player;

typedef struct {
    float x, y;
    float vx, vy;
    int rayon;
    int niveau; // 3 = Grande, 2 = Moyenne, 1 = Petite
    bool actif;
    int color;
} Bulle;

typedef struct {
    float x, y;
    float vy;
    bool actif;
} Projectile;

typedef struct {
    int x, y, w, h;
    int couleur;
} Plateforme;

// --- VARIABLES GLOBALES ---
BITMAP *page;
Player joueur;
Projectile tab_tirs[MAX_PROJECTILES];
Plateforme map[MAX_PLATEFORMES];
Bulle tab_bulles[MAX_BULLES];
int tempoglobale = 15;

// --- PROTOTYPES ---
void initialisation_allegro();
void init_game();
void update_game();
void render_game();
void cleanup_game();
void init_tirs();
void tirer(int x, int y);
void update_projectiles();
void init_map();
void check_collision_plateformes();
void init_bulles();
void ajouter_bulle(float x, float y, float vx, float vy, int niveau);
void update_bulles();
void check_collisions_tirs_bulles();

// --- PROGRAMME PRINCIPAL ---
int main() {
    initialisation_allegro();
    init_game();

    while (!key[KEY_ESC]) {
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

    // Initialisation Joueur
    joueur.taille = SCREEN_W / 20;
    joueur.x = SCREEN_W / 2;
    joueur.y = SCREEN_H - joueur.taille;
    joueur.speed_x = 8;
    joueur.vy = 0;
    joueur.is_jumping = false;
    joueur.color = makecol(0, 0, 255);
    joueur.reload_timer = 0;

    init_tirs();
    init_map();
    init_bulles();

    // Apparition de la première bulle (Niveau 3 = Grosse bulle)
    ajouter_bulle(SCREEN_W / 2, 100, 3.0f, 0, 3);
}

void init_tirs() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        tab_tirs[i].actif = false;
    }
}

void init_bulles() {
    for (int i = 0; i < MAX_BULLES; i++) {
        tab_bulles[i].actif = false;
    }
}

void init_map() {
    map[0] = (Plateforme){100, 450, 150, 20, makecol(0, 255, 100)};
    map[1] = (Plateforme){500, 400, 200, 20, makecol(0, 255, 100)};
    map[2] = (Plateforme){300, 300, 120, 20, makecol(0, 255, 100)};
    map[3] = (Plateforme){50, 200, 100, 20, makecol(0, 255, 100)};
    map[4] = (Plateforme){600, 250, 100, 20, makecol(0, 255, 100)};
    map[5] = (Plateforme){350, 150, 180, 20, makecol(0, 255, 100)};
}

void ajouter_bulle(float x, float y, float vx, float vy, int niveau) {
    for (int i = 0; i < MAX_BULLES; i++) {
        if (!tab_bulles[i].actif) {
            tab_bulles[i].x = x;
            tab_bulles[i].y = y;
            tab_bulles[i].vx = vx;
            tab_bulles[i].vy = vy;
            tab_bulles[i].niveau = niveau;
            tab_bulles[i].rayon = niveau * 15; // Taille dépendante du niveau
            tab_bulles[i].color = makecol(255, 0, 0); // Bulles rouges
            tab_bulles[i].actif = true;
            break;
        }
    }
}

void update_game() {
    // 1. Contrôles horizontaux
    if (key[KEY_LEFT]) joueur.x -= joueur.speed_x;
    if (key[KEY_RIGHT]) joueur.x += joueur.speed_x;

    // 2. Saut et Gravité
    if (key[KEY_UP] && !joueur.is_jumping) {
        joueur.vy = -JUMP_FORCE;
        joueur.is_jumping = true;
    }
    joueur.vy += GRAVITY;
    joueur.y += (int)joueur.vy;

    // 3. Collisions plateformes
    check_collision_plateformes();

    // 4. Tir
    if (joueur.reload_timer > 0) joueur.reload_timer--;
    if (key[KEY_SPACE] && joueur.reload_timer == 0) {
        tirer(joueur.x + joueur.taille / 2, joueur.y);
        joueur.reload_timer = RELOAD_TIME;
    }

    // 5. Mise à jour des entités
    update_projectiles();
    update_bulles();
    check_collisions_tirs_bulles();

    // 6. Contraintes bords d'écran (Joueur)
    if (joueur.x < 0) joueur.x = 0;
    if (joueur.x + joueur.taille > SCREEN_W) joueur.x = SCREEN_W - joueur.taille;

    // 7. Collision sol (Joueur)
    if (joueur.y + joueur.taille >= SCREEN_H) {
        joueur.y = SCREEN_H - joueur.taille;
        joueur.vy = 0;
        joueur.is_jumping = false;
    }
}

void update_bulles() {
    for (int i = 0; i < MAX_BULLES; i++) {
        if (tab_bulles[i].actif) {
            tab_bulles[i].vy += BULLE_GRAVITY;
            tab_bulles[i].x += tab_bulles[i].vx;
            tab_bulles[i].y += tab_bulles[i].vy;

            // Rebond sur les murs
            if (tab_bulles[i].x - tab_bulles[i].rayon < 0) {
                tab_bulles[i].x = tab_bulles[i].rayon;
                tab_bulles[i].vx = -tab_bulles[i].vx;
            } else if (tab_bulles[i].x + tab_bulles[i].rayon > SCREEN_W) {
                tab_bulles[i].x = SCREEN_W - tab_bulles[i].rayon;
                tab_bulles[i].vx = -tab_bulles[i].vx;
            }

            // Rebond sur le sol
            if (tab_bulles[i].y + tab_bulles[i].rayon > SCREEN_H) {
                tab_bulles[i].y = SCREEN_H - tab_bulles[i].rayon;
                // L'impulsion de rebond varie selon la taille de la bulle
                tab_bulles[i].vy = -8.0f - (tab_bulles[i].niveau * 1.5f);
            }

            // Rebond plafond (optionnel mais utile pour éviter qu'elles sortent en haut)
            if (tab_bulles[i].y - tab_bulles[i].rayon < 0) {
                tab_bulles[i].y = tab_bulles[i].rayon;
                tab_bulles[i].vy = -tab_bulles[i].vy;
            }
        }
    }
}

void check_collisions_tirs_bulles() {
    for (int b = 0; b < MAX_BULLES; b++) {
        if (!tab_bulles[b].actif) continue;

        for (int t = 0; t < MAX_PROJECTILES; t++) {
            if (!tab_tirs[t].actif) continue;

            // Calcul de la distance entre le haut du tir et le centre de la bulle
            float dx = tab_bulles[b].x - tab_tirs[t].x;
            float dy = tab_bulles[b].y - tab_tirs[t].y;
            float dist = sqrt(dx * dx + dy * dy);

            // Si la distance est inférieure au rayon, il y a collision
            if (dist < tab_bulles[b].rayon) {
                tab_tirs[t].actif = false;  // Détruire le tir
                tab_bulles[b].actif = false; // Détruire la bulle

                // Si la bulle n'est pas de la taille minimale, elle se divise
                if (tab_bulles[b].niveau > 1) {
                    // Les nouvelles bulles partent avec une vitesse horizontale opposée (trajectoires divergentes)
                    ajouter_bulle(tab_bulles[b].x, tab_bulles[b].y, -3.5f, -5.0f, tab_bulles[b].niveau - 1);
                    ajouter_bulle(tab_bulles[b].x, tab_bulles[b].y, 3.5f, -5.0f, tab_bulles[b].niveau - 1);
                }
                break; // On arrête de tester ce tir puisqu'il est détruit
            }
        }
    }
}

void check_collision_plateformes() {
    for (int i = 0; i < MAX_PLATEFORMES; i++) {
        if (joueur.x + joueur.taille > map[i].x && joueur.x < map[i].x + map[i].w) {
            if (joueur.y + joueur.taille >= map[i].y &&
                joueur.y + joueur.taille <= map[i].y + map[i].h + (int)joueur.vy) {
                if (joueur.vy > 0) {
                    joueur.y = map[i].y - joueur.taille;
                    joueur.vy = 0;
                    joueur.is_jumping = false;
                }
            }
        }
    }
}

void tirer(int x, int y) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!tab_tirs[i].actif) {
            tab_tirs[i].x = (float)x;
            tab_tirs[i].y = (float)y;
            tab_tirs[i].vy = -PROJECTILE_SPEED;
            tab_tirs[i].actif = true;
            break;
        }
    }
}

void update_projectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (tab_tirs[i].actif) {
            tab_tirs[i].y += tab_tirs[i].vy;
            if (tab_tirs[i].y < 0) tab_tirs[i].actif = false;
        }
    }
}

void render_game() {
    clear_to_color(page, makecol(255, 255, 255));

    // Zone d'information (HUD)
    rectfill(page, 0, 0, SCREEN_W, 40, makecol(200, 200, 200));
    textprintf_ex(page, font, 10, 15, makecol(0, 0, 0), -1, "PROJET: SUPER BULLES | TIME: 95");

    // Dessin des Plateformes
    for (int i = 0; i < MAX_PLATEFORMES; i++) {
        rectfill(page, map[i].x, map[i].y, map[i].x + map[i].w, map[i].y + map[i].h, map[i].couleur);
    }

    // Dessin des Projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (tab_tirs[i].actif) {
            rectfill(page, (int)tab_tirs[i].x - 2, (int)tab_tirs[i].y,
                           (int)tab_tirs[i].x + 2, (int)tab_tirs[i].y + 10, makecol(255, 0, 0));
        }
    }

    // Dessin des Bulles
    for (int i = 0; i < MAX_BULLES; i++) {
        if (tab_bulles[i].actif) {
            circlefill(page, (int)tab_bulles[i].x, (int)tab_bulles[i].y, tab_bulles[i].rayon, tab_bulles[i].color);
        }
    }

    // Dessin du joueur
    rectfill(page, (int)joueur.x, (int)joueur.y,
                   (int)joueur.x + joueur.taille, (int)joueur.y + joueur.taille, joueur.color);

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
        allegro_message("Erreur GFX : %s", allegro_error);
        exit(EXIT_FAILURE);
    }
}
