#ifndef TYPES_H
#define TYPES_H

#include <allegro.h>
#include <stdbool.h>

/* =========================================================
   TYPES.H — Structures et constantes partagées
   Super Bulles · Style Japon / Naruto · Allegro 4.4
   ========================================================= */

/* ---------------------------------------------------------
   DIMENSIONS
   --------------------------------------------------------- */
#define WINDOW_W        1024
#define WINDOW_H         700
#define GAME_H           620
#define GAME_Y            10
#define HUD_H             80
#define NB_NIVEAUX         5
#define FPS               60
#define MAX_SAUVEGARDES   20

/* ---------------------------------------------------------
   JOUEUR
   --------------------------------------------------------- */
#define PLAYER_W      48
#define PLAYER_H      64
#define PLAYER_SPEED  4.5f

/* ---------------------------------------------------------
   PROJECTILES
   --------------------------------------------------------- */
#define PROJ_W    10
#define PROJ_H    24
#define PROJ_SPEED 12

/* ---------------------------------------------------------
   BULLES
   --------------------------------------------------------- */
#define BULLE_RAYON_BASE  18
#define GRAVITY           0.25f
#define BOUNCE_VY_BASE   -9.0f

/* ---------------------------------------------------------
   SCORES
   --------------------------------------------------------- */
#define SCORE_BULLE_1   100
#define SCORE_BULLE_2   200
#define SCORE_BULLE_3   400
#define SCORE_BULLE_4   800

/* ---------------------------------------------------------
   ÉTATS ÉCRAN
   --------------------------------------------------------- */
typedef enum {
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

/* ---------------------------------------------------------
   ARMES
   --------------------------------------------------------- */
typedef enum {
    ARME_SIMPLE = 0,
    ARME_DOUBLE,
    ARME_TRIPLE,
    ARME_ECLAIR
} TypeArme;

/* ---------------------------------------------------------
   BONUS
   --------------------------------------------------------- */
#define NB_TYPES_BONUS 5
typedef enum {
    BONUS_ARME_DOUBLE = 0,
    BONUS_ARME_TRIPLE,
    BONUS_ARME_ECLAIR,
    BONUS_TEMPS,
    BONUS_INVINCIBLE
} TypeBonus;

/* ---------------------------------------------------------
   STRUCTS ENTITÉS
   --------------------------------------------------------- */

typedef enum {
    JOUEUR_IDLE = 0,
    JOUEUR_MARCHE,
    JOUEUR_ATTAQUE,
    JOUEUR_TOUCHE,
    JOUEUR_MORT
} EtatJoueur;

typedef struct {
    float      x, y, vx;
    TypeArme   arme;
    int        arme_timer;
    bool       invincible;
    int        invincible_timer;
    bool       vivant;
    int        vies;
    int        score;
    char       pseudo[32];
    int        id;
    int        tir_cooldown;
    int        direction;
    EtatJoueur etat_anim;
    int        anim_frame;
    int        anim_timer;
} Joueur;

typedef struct Projectile {
    float  x, y, vy;
    bool   actif;
    TypeArme type;
    struct Projectile *suivant;
} Projectile;

typedef struct Bulle {
    float x, y, vx, vy;
    int   taille;
    bool  lance_eclairs;
    int   eclair_timer;
    bool  active;
    int   choc_timer;
    struct Bulle *suivante;
} Bulle;

typedef struct Eclair {
    float x, y, vy;
    bool  actif;
    struct Eclair *suivant;
} Eclair;

#define MAX_FIREBALLS 8

typedef struct {
    float x, y;
    float vx, vy;
    bool  actif;
    int   frame_anim;
} Fireball;

typedef struct BonusItem {
    float     x, y;
    float     vy;
    TypeBonus type;
    bool      actif;
    int       duree_vie;
    struct BonusItem *suivant;
} BonusItem;

typedef enum {
    BOSS_IDLE = 0,
    BOSS_ATTAQUE,
    BOSS_ENRAGE,
    BOSS_MORT_ANIM
} EtatBoss;

typedef struct {
    float    x, y, vx, vy;
    int      pv, pv_max;
    bool     actif;
    int      spawn_timer;
    int      hit_flash;
    EtatBoss etat_anim;
    int      anim_frame;
    int      anim_timer;
} Boss;

typedef struct {
    int  numero;
    int  temps_restant;
    int  frame_timer;
    bool en_cours;
    bool gagne;
    int  countdown;
} EtatNiveau;

typedef struct {
    char pseudo[32];
    int  dernier_niveau;
    int  meilleur_score;
    int  nb_joueurs;
} Sauvegarde;

/* ---------------------------------------------------------
   RESSOURCES GRAPHIQUES
   Structure correspondant exactement a graphismes.c
   --------------------------------------------------------- */
typedef struct {
    /* Fonds menus */
    BITMAP *fond_menu;
    BITMAP *fond_jouer;
    BITMAP *panneau_geant;
    BITMAP *choix_joueurs_fond;
    BITMAP *saisie_pseudo_fond;
    BITMAP *pause_fond;
    BITMAP *fin_niveau_fond;
    BITMAP *victoire_fond;
    BITMAP *game_over_fond;
    BITMAP *hud_fond;

    /* Fonds niveaux */
    BITMAP *fond_niveaux[NB_NIVEAUX];

    /* Boutons */
    BITMAP *btn_jouer_idle,    *btn_jouer_hover;
    BITMAP *btn_regles_idle,   *btn_regles_hover;
    BITMAP *btn_options_idle,  *btn_options_hover;
    BITMAP *btn_credits_idle,  *btn_credits_hover;
    BITMAP *btn_nouvelle_idle, *btn_nouvelle_hover;
    BITMAP *btn_charger_idle,  *btn_charger_hover;
    BITMAP *btn_retour;

    /* Joueur 1 */
    BITMAP *joueur_idle;
    BITMAP *joueur_run   [6];
    BITMAP *joueur_marche[4];   /* alias de joueur_run[0..3] */

    /* Joueur 2 */
    BITMAP *joueur2_idle;
    BITMAP *joueur2_run[6];

    /* Projectiles */
    BITMAP *kunai;
    BITMAP *eclair_spr;
    BITMAP *eclair_anim[4];
    BITMAP *fireball   [4];

    /* Bulles */
    BITMAP *bulle      [4];   /* index 1..3 utilisés */
    BITMAP *bulle_eclair[4];
    BITMAP *bulle_choc  [4];

    /* Bonus */
    BITMAP *bonus[NB_TYPES_BONUS];

    /* Boss */
    BITMAP *boss_idle;
    BITMAP *boss_hit;
    BITMAP *boss_run[4];

} Ressources;

/* ---------------------------------------------------------
   IHM
   --------------------------------------------------------- */
typedef struct {
    int     x, y, w, h;
    BITMAP *img_idle;
    BITMAP *img_hover;
    int     is_hovered;
} Bouton;

typedef struct {
    int  souris_x, souris_y;
    bool clic_gauche;
    bool fleche_gauche, fleche_droite;
    bool tir;
    bool echap;
    bool entree;
    bool pause_key;
    char texte_saisi;
    bool backspace;
} EntreesFrame;

typedef struct {
    int clic;
    int tir;
} Cooldowns;

#endif /* TYPES_H */