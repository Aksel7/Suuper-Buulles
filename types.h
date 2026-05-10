#ifndef TYPES_H
#define TYPES_H

#include <allegro.h>
#include <stdbool.h>

/* =========================================================
   TYPES.H
   Structures + constantes globales
   ========================================================= */

/* =========================================================
   DIMENSIONS
   ========================================================= */

#define WINDOW_W 1024
#define WINDOW_H 700

#define GAME_H 620
#define GAME_Y 10

#define HUD_H 80

#define FPS 60

#define NB_NIVEAUX 5
#define NB_TYPES_BONUS 5

#define MAX_SAUVEGARDES 20
#define MAX_FIREBALLS 8

/* =========================================================
   JOUEUR
   ========================================================= */

#define PLAYER_W 48
#define PLAYER_H 64

#define PLAYER_SPEED 4.5f

/* =========================================================
   PROJECTILES
   ========================================================= */

#define PROJ_W 10
#define PROJ_H 24

#define PROJ_SPEED 12

/* =========================================================
   BULLES
   ========================================================= */

#define BULLE_RAYON_BASE 18

#define GRAVITY 0.25f
#define BOUNCE_VY_BASE -9.0f

/* =========================================================
   SCORES
   ========================================================= */

#define SCORE_BULLE_1 100
#define SCORE_BULLE_2 200
#define SCORE_BULLE_3 400
#define SCORE_BULLE_4 800

/* =========================================================
   ETATS ECRAN
   ========================================================= */

typedef enum
{
    ETAT_MENU_PRINCIPAL = 0,
    ETAT_MENU_JOUER,
    ETAT_MENU_REGLES,
    ETAT_MENU_OPTIONS,
    ETAT_MENU_CREDITS,
    ETAT_SAISIE_PSEUDO,
    ETAT_JEU,
    ETAT_PAUSE,
    ETAT_FIN_NIVEAU,
    ETAT_VICTOIRE,
    ETAT_GAME_OVER,
    ETAT_QUITTER

} EtatEcran;

/* =========================================================
   ARMES
   ========================================================= */

typedef enum
{
    ARME_SIMPLE = 0,
    ARME_DOUBLE,
    ARME_TRIPLE,
    ARME_ECLAIR

} TypeArme;

/* =========================================================
   BONUS
   ========================================================= */

typedef enum
{
    BONUS_ARME_DOUBLE = 0,
    BONUS_ARME_TRIPLE,
    BONUS_ARME_ECLAIR,
    BONUS_TEMPS,
    BONUS_INVINCIBLE

} TypeBonus;

/* =========================================================
   JOUEUR
   ========================================================= */

typedef struct
{
    float x;
    float y;

    float vx;

    TypeArme arme;

    int arme_timer;

    bool invincible;
    int invincible_timer;

    bool vivant;

    int score;

    char pseudo[32];

    int id;

    int tir_cooldown;

} Joueur;

/* =========================================================
   PROJECTILE
   ========================================================= */

typedef struct Projectile
{
    float x;
    float y;

    float vy;

    bool actif;

    TypeArme type;

    struct Projectile *suivant;

} Projectile;

/* =========================================================
   BULLE
   ========================================================= */

typedef struct Bulle
{
    float x;
    float y;

    float vx;
    float vy;

    int taille;

    bool lance_eclairs;

    int eclair_timer;

    bool active;

    int choc_timer;

    struct Bulle *suivante;

} Bulle;

/* =========================================================
   ECLAIR
   ========================================================= */

typedef struct Eclair
{
    float x;
    float y;

    float vy;

    bool actif;

    struct Eclair *suivant;

} Eclair;

/* =========================================================
   FIREBALL
   ========================================================= */

typedef struct
{
    float x;
    float y;

    float vx;
    float vy;

    bool actif;

    int frame_anim;

} Fireball;

/* =========================================================
   BONUS ITEM
   ========================================================= */

typedef struct BonusItem
{
    float x;
    float y;

    float vy;

    TypeBonus type;

    bool actif;

    int duree_vie;

    struct BonusItem *suivant;

} BonusItem;

/* =========================================================
   BOSS
   ========================================================= */

typedef struct
{
    float x;
    float y;

    float vx;
    float vy;

    int pv;
    int pv_max;

    bool actif;

    int spawn_timer;

    int hit_flash;

} Boss;

/* =========================================================
   ETAT NIVEAU
   ========================================================= */

typedef struct
{
    int numero;

    int temps_restant;

    int frame_timer;

    bool en_cours;

    bool gagne;

    int countdown;

} EtatNiveau;

/* =========================================================
   SAUVEGARDE
   ========================================================= */

typedef struct
{
    char pseudo[32];

    int dernier_niveau;

    int meilleur_score;

} Sauvegarde;

/* =========================================================
   RESSOURCES
   ========================================================= */

typedef struct
{
    /* =========================
       MENUS / INTERFACES
       ========================= */

    BITMAP *fond_menu;
    BITMAP *fond_jouer;
    BITMAP *panneau_geant;

    BITMAP *btn_jouer_idle;
    BITMAP *btn_jouer_hover;

    BITMAP *btn_regles_idle;
    BITMAP *btn_regles_hover;

    BITMAP *btn_options_idle;
    BITMAP *btn_options_hover;

    BITMAP *btn_credits_idle;
    BITMAP *btn_credits_hover;

    BITMAP *btn_nouvelle_idle;
    BITMAP *btn_nouvelle_hover;

    BITMAP *btn_charger_idle;
    BITMAP *btn_charger_hover;

    BITMAP *btn_retour;

    /* =========================
       FONDS JEU
       ========================= */

    BITMAP *fond_niveaux[NB_NIVEAUX];

    BITMAP *hud_fond;

    BITMAP *pause_fond;

    BITMAP *fin_niveau_fond;

    BITMAP *victoire_fond;

    BITMAP *game_over_fond;

    BITMAP *choix_joueurs_fond;

    BITMAP *saisie_pseudo_fond;

    /* =========================
       JOUEUR 1
       ========================= */

    BITMAP *joueur_idle;

    BITMAP *joueur_marche[4];

    BITMAP *joueur_run[6];

    BITMAP *joueur_jump;

    BITMAP *joueur_tir;

    /* =========================
       JOUEUR 2
       ========================= */

    BITMAP *joueur2_idle;

    BITMAP *joueur2_run[6];

    BITMAP *joueur2_jump;

    BITMAP *joueur2_tir;

    /* =========================
       PROJECTILES
       ========================= */

    BITMAP *kunai;

    BITMAP *eclair_spr;

    BITMAP *eclair_anim[4];

    BITMAP *fireball[4];

    /* =========================
       BULLES
       ========================= */

    BITMAP *bulle[4];

    BITMAP *bulle_eclair[4];

    BITMAP *bulle_choc[4];

    /* =========================
       BONUS
       ========================= */

    BITMAP *bonus[NB_TYPES_BONUS];

    /* =========================
       BOSS
       ========================= */

    BITMAP *boss_idle;

    BITMAP *boss_hit;

    BITMAP *boss_run[4];

} Ressources;

/* =========================================================
   BOUTON
   ========================================================= */

typedef struct
{
    int x;
    int y;

    int w;
    int h;

    BITMAP *img_idle;
    BITMAP *img_hover;

    int is_hovered;

} Bouton;

/* =========================================================
   ENTREES
   ========================================================= */

typedef struct
{
    int souris_x;
    int souris_y;

    bool clic_gauche;

    bool fleche_gauche;
    bool fleche_droite;

    bool tir;

    bool echap;

    bool entree;

    bool pause_key;

    char texte_saisi;

    bool backspace;

} EntreesFrame;

/* =========================================================
   COOLDOWNS
   ========================================================= */

typedef struct
{
    int clic;
    int tir;

} Cooldowns;

#endif