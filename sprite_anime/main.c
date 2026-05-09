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
    BITMAP *sprites[10];
    int frame_actuelle;
} Player;

typedef struct  {
    int x, y;
    int taille;
} Bulles;

typedef struct  {
    int x, y;
    int taille;
    int speed_x;
    float vy;
    int color;
} Projectiles;

// --- VARIABLES GLOBALES ---

#define MAX_PROJECTILES 50
Projectiles munitions[MAX_PROJECTILES];
int nb_projectiles = 0;

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
void init_joueur();
void init_game() {
    page = create_bitmap(SCREEN_W, SCREEN_H);
    //fond = load_bitmap_check("bg.bmp");
    //bmp = load_bitmap("image.pcx", palette);
    //
    //blit
    init_joueur();
    //init_obstacles();
}

void init_joueur(){
    joueur.taille = 40;
    joueur.x = SCREEN_W / 2;// commencer milieu
    joueur.y = SCREEN_H - joueur.taille; // commencer au sol
    joueur.speed_x = 6;
    joueur.vy = 0;
    joueur.is_jumping = false;
    char nom[50];
    for (int i = 0; i < 10; i++) {
        sprintf(nom, "images/player%d.bmp", i); // Assure-toi que le dossier images existe
        joueur.sprites[i] = load_bitmap(nom, NULL);
    }
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
    // sol = SCREEN_H
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
    clear_to_color(page, makecol(255, 255, 255));

    // Vérifie si le sprite existe avant de l'afficher
    if (joueur.sprites[joueur.frame_actuelle] != NULL) {
        draw_sprite(page, joueur.sprites[joueur.frame_actuelle], joueur.x, joueur.y);
    } else {
        // Fallback : si l'image n'est pas chargée, on dessine un rectangle pour pas être aveugle
        rectfill(page, joueur.x, joueur.y, joueur.x + joueur.taille, joueur.y + joueur.taille, makecol(0,0,255));
    }

    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void cleanup_game() {
    if (page != NULL) destroy_bitmap(page);
    //destroy_bitmap(fond);
    //destroy_bitmap(buffer);
    allegro_exit();
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

void trajB(Bulles tabBulle[]) {
    for (int i = 0; i < 15; i++) {
        tabBulle[i].y = tabBulle[i].y - 10;
    }
    //affichagebulle(tabBulle[i]);
}

void animer_personnage(Player p) {
    static int compteur = 0;
    compteur++;

    if (compteur > 5) {
        // Utilise -> pour accéder aux membres d'un pointeur
        p.frame_actuelle = (p.frame_actuelle + 1) % 10;
        compteur = 0;
    }
}

////////////////tir de bulle

void tirer() {
    if (nb_projectiles < MAX_PROJECTILES) {
        munitions[nb_projectiles].x = joueur.x + (joueur.taille / 2);
        munitions[nb_projectiles].y = joueur.y;
        munitions[nb_projectiles].vy = -10.0f;//syntaxe pour float
        munitions[nb_projectiles].color = makecol(255, 0, 0);
        nb_projectiles++;
    }
}

void update_projectiles() {
    for (int i = 0; i < nb_projectiles; i++) {//iterer tout les tirs
        munitions[i].y += munitions[i].vy;//bouge de sa vitesse (plus dynamique que prochain point)

        if (munitions[i].y < 0) {
            munitions[i] = munitions[nb_projectiles - 1];
            nb_projectiles--;
            i--;
        }
    }
}


////////////////////////////////////////////A Review//////////////////////////////////////
//////////////systeme de collisiom ///
bool collision_HB(int obj1x, int obj1y, int taille1, int obj2x, int obj2y, int taille2) {
    if (obj1x < obj2x + taille2 && obj1x + taille1 > obj2x) {//collision horizontale
        if (obj1y < obj2y + taille2 && obj1y + taille1 > obj2y) {//collision verticale

            return true; // Collision trouver
        }
    }
    return false; // RAS
}

///
void check_coll_tir() {
    for (int i = 0; i < nb_projectiles; i++) {//check des projectil 
        for (int j = 0; j < nb_ennemis; j++) {

            if (collision_HB(munitions[i].x, munitions[i].y, munitions[i].taille, 
                             ennemis[j].x, ennemis[j].y, ennemis[j].taille))// pos et taille des projectile/mechant #i
            {//toucher-couler
                ennemis[j] = ennemis[nb_ennemis - 1];//remove de list mechant
                nb_ennemis--;

                munitions[i] = munitions[nb_projectiles - 1];//remove de projectile
                nb_projectiles--;

                
                i--;//iter
                break;
            }//continue
        }
    }
}
