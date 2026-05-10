#ifndef JOUEUR_H
#define JOUEUR_H

#include <allegro.h>
#include "niveaux.h"

/* =========================================================
   JOUEUR.H — Entité joueur : mouvements, armes, projectiles
   Oni no Michi — Style japonais / pixel art
   ========================================================= */


/* ---------------------------------------------------------
   CONSTANTES
   --------------------------------------------------------- */

/* Physique */
#define JOUEUR_VITESSE          3.5f
#define JOUEUR_VITESSE_DASH     9.0f
#define JOUEUR_SAUT_FORCE      -7.5f
#define JOUEUR_GRAVITE          0.4f
#define JOUEUR_GRAVITE_MAX      12.0f
#define JOUEUR_DASH_DUREE       12      /* frames */
#define JOUEUR_DASH_COOLDOWN    45      /* frames */
#define JOUEUR_COYOTE_FRAMES    6       /* frames après bord du sol */
#define JOUEUR_SAUT_BUFFER      8       /* frames anticipation saut */

/* Stats */
#define JOUEUR_VIE_MAX          100
#define JOUEUR_MANA_MAX         80
#define JOUEUR_INVINCIBLE_DUR   60      /* frames après dégâts */

/* Hitbox */
#define JOUEUR_LARGEUR          20
#define JOUEUR_HAUTEUR          28

/* Projectiles joueur */
#define MAX_PROJ_JOUEUR         16

/* Armes */
#define NB_ARMES                4

/* Cooldowns tir (frames) */
#define CD_SHURIKEN             15
#define CD_ARC                  25
#define CD_KUNAI                10
#define CD_MAGIE                35

/* Vitesses projectiles */
#define VITESSE_SHURIKEN        7.0f
#define VITESSE_FLECHE          9.0f
#define VITESSE_KUNAI           8.0f
#define VITESSE_MAGIE           5.0f

/* Dégâts */
#define DEGATS_SHURIKEN         12
#define DEGATS_FLECHE           20
#define DEGATS_KUNAI            8
#define DEGATS_MAGIE            30


/* ---------------------------------------------------------
   ENUMS
   --------------------------------------------------------- */

typedef enum {
    DIR_DROITE = 1,
    DIR_GAUCHE = -1
} Direction;

typedef enum {
    ARME_SHURIKEN = 0,   /* Corps à corps + lancer rapide */
    ARME_ARC      = 1,   /* Flèche lente, gros dégâts     */
    ARME_KUNAI    = 2,   /* Rafale rapide, faibles dégâts  */
    ARME_MAGIE    = 3    /* Boule de feu, coûte du mana   */
} TypeArme;

typedef enum {
    PROJ_SHURIKEN = 0,
    PROJ_FLECHE   = 1,
    PROJ_KUNAI    = 2,
    PROJ_MAGIE    = 3
} TypeProjectile;

typedef enum {
    ANIM_IDLE    = 0,
    ANIM_MARCHE  = 1,
    ANIM_SAUT    = 2,
    ANIM_CHUTE   = 3,
    ANIM_DASH    = 4,
    ANIM_ATTAQUE = 5,
    ANIM_TOUCHE  = 6,
    ANIM_MORT    = 7,
    NB_ETATS_ANIM
} EtatAnimation;


/* ---------------------------------------------------------
   STRUCT PROJECTILE JOUEUR
   --------------------------------------------------------- */

typedef struct {
    float          x, y;
    float          vx, vy;
    int            actif;
    TypeProjectile type;
    int            degats;
    int            frame_vie;    /* durée de vie en frames  */
    int            frame_anim;   /* compteur animation      */
    Direction      dir;
    BITMAP        *sprite;       /* frame courante (non owned) */
} ProjetileJoueur;


/* ---------------------------------------------------------
   STRUCT JOUEUR
   --------------------------------------------------------- */

typedef struct {

    /* --- position & physique --- */
    float      x, y;
    float      vx, vy;
    int        au_sol;

    /* --- dimensions hitbox --- */
    int        largeur, hauteur;

    /* --- direction regardée --- */
    Direction  dir;

    /* --- stats --- */
    int        vie, vie_max;
    int        mana, mana_max;

    /* --- état combat --- */
    int        invincible;        /* compteur frames d'invincibilité */
    int        mort;

    /* --- mouvement spéciaux --- */
    int        dash_actif;
    int        dash_timer;        /* frames restantes du dash         */
    int        dash_cooldown;     /* cooldown restant                 */
    Direction  dash_dir;

    int        coyote_timer;      /* frames de grâce après le bord    */
    int        saut_buffer;       /* anticipation saut                */
    int        nb_sauts_restants; /* double saut                      */

    /* --- arme courante & cooldown --- */
    TypeArme   arme;
    int        cooldown_tir;

    /* --- pool de projectiles --- */
    ProjetileJoueur projectiles[MAX_PROJ_JOUEUR];

    /* --- animation --- */
    EtatAnimation  etat_anim;
    int            frame_anim;    /* compteur interne animation       */
    int            anim_timer;    /* frames avant prochaine frame     */

    /* --- sprites (chargés par charger_sprites_joueur) --- */
    BITMAP    **frames[NB_ETATS_ANIM]; /* tableau de frames par état  */
    int         nb_frames[NB_ETATS_ANIM];
    BITMAP     *sprites_proj[NB_ARMES]; /* 1 sprite par type proj      */

} Joueur;


/* ---------------------------------------------------------
   PROTOTYPES
   --------------------------------------------------------- */

/* --- Init / Libération --- */
void init_joueur        (Joueur *j, float x, float y);
void liberer_joueur     (Joueur *j);
int  charger_sprites_joueur(Joueur *j);

/* --- Update principal --- */
void update_joueur      (Joueur *j, NiveauConfig *niveau);

/* --- Mouvements --- */
void joueur_deplacer    (Joueur *j, NiveauConfig *niveau);
void joueur_sauter      (Joueur *j);
void joueur_dash        (Joueur *j);
void joueur_appliquer_gravite(Joueur *j);
void joueur_resoudre_collisions(Joueur *j, NiveauConfig *niveau);

/* --- Armes & tir --- */
void joueur_changer_arme(Joueur *j, TypeArme arme);
void joueur_tirer       (Joueur *j);
void joueur_attaque_corps(Joueur *j);    /* hitbox mêlée shuriken */

/* --- Projectiles --- */
void update_projectiles_joueur(Joueur *j, NiveauConfig *niveau);
void draw_projectiles_joueur  (BITMAP *buffer, Joueur *j);
ProjetileJoueur *trouver_slot_proj(Joueur *j);

/* --- Dégâts & vie --- */
void joueur_prendre_degats(Joueur *j, int degats);
void joueur_soigner       (Joueur *j, int soin);
void joueur_depenser_mana (Joueur *j, int cout);
void joueur_recharger_mana(Joueur *j, int quantite);

/* --- Animation --- */
void update_animation_joueur(Joueur *j);
void draw_joueur            (BITMAP *buffer, Joueur *j);
void draw_joueur_hud        (BITMAP *buffer, Joueur *j);

/* --- Utilitaires --- */
int  joueur_hitbox_x      (const Joueur *j);
int  joueur_hitbox_y      (const Joueur *j);
int  joueur_centre_x      (const Joueur *j);
int  joueur_centre_y      (const Joueur *j);
int  joueur_est_vivant    (const Joueur *j);

#endif /* JOUEUR_H */