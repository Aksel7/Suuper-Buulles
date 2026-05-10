#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "joueurs.h"
#include "niveaux.h"

/* =========================================================
   JOUEUR.C — Mouvements, armes, projectiles, animation
   Oni no Michi — Style japonais / pixel art
   ========================================================= */


/* ---------------------------------------------------------
   Couleurs HUD (palette japonaise)
   --------------------------------------------------------- */
#define COL_VIE_FOND    makecol(50,  10, 10 )
#define COL_VIE_PLEIN   makecol(192, 57, 43 )   /* aka   */
#define COL_MANA_FOND   makecol(10,  20, 50 )
#define COL_MANA_PLEIN  makecol(27,  58, 92 )   /* kon   */
#define COL_OR          makecol(201,168, 76 )   /* kin   */
#define COL_BLANC       makecol(245,239,224 )   /* washi */
#define COL_OMBRE       makecol(26,  20, 16 )   /* sumi  */


/* =========================================================
   INIT / LIBÉRATION
   ========================================================= */

void init_joueur(Joueur *j, float x, float y)
{
    if (j == NULL) return;

    memset(j, 0, sizeof(Joueur));

    j->x         = x;
    j->y         = y;
    j->largeur   = JOUEUR_LARGEUR;
    j->hauteur   = JOUEUR_HAUTEUR;

    j->vie       = JOUEUR_VIE_MAX;
    j->vie_max   = JOUEUR_VIE_MAX;
    j->mana      = JOUEUR_MANA_MAX;
    j->mana_max  = JOUEUR_MANA_MAX;

    j->dir       = DIR_DROITE;
    j->arme      = ARME_SHURIKEN;

    j->nb_sauts_restants = 2;   /* double saut autorisé */

    j->etat_anim = ANIM_IDLE;
}

int charger_sprites_joueur(Joueur *j)
{
    /*
       Charge les sprites depuis le disque.
       Convention de nommage des fichiers BMP/PCX :
         sprites/joueur/idle_0.pcx  idle_1.pcx  ...
         sprites/joueur/marche_0.pcx ...
         sprites/joueur/saut_0.pcx
         sprites/joueur/chute_0.pcx
         sprites/joueur/dash_0.pcx
         sprites/joueur/attaque_0.pcx ... attaque_4.pcx
         sprites/joueur/touche_0.pcx
         sprites/joueur/mort_0.pcx ... mort_5.pcx
         sprites/joueur/proj_shuriken.pcx
         sprites/joueur/proj_fleche.pcx
         sprites/joueur/proj_kunai.pcx
         sprites/joueur/proj_magie.pcx
    */
    static const char *noms[NB_ETATS_ANIM] = {
        "idle", "marche", "saut", "chute",
        "dash", "attaque", "touche", "mort"
    };
    static const int nb[NB_ETATS_ANIM] = {
        4, 6, 2, 2, 3, 5, 2, 6
    };

    int e, f;
    char chemin[128];

    for (e = 0; e < NB_ETATS_ANIM; e++) {
        j->nb_frames[e]  = nb[e];
        j->frames[e]     = (BITMAP **)malloc(nb[e] * sizeof(BITMAP *));
        if (j->frames[e] == NULL) return 0;

        for (f = 0; f < nb[e]; f++) {
            sprintf(chemin, "sprites/joueur/%s_%d.pcx", noms[e], f);
            j->frames[e][f] = load_bitmap(chemin, NULL);
            if (j->frames[e][f] == NULL) {
                /* Fallback : carré coloré si fichier absent */
                j->frames[e][f] = create_bitmap(JOUEUR_LARGEUR, JOUEUR_HAUTEUR);
                clear_to_color(j->frames[e][f], makecol(201, 168, 76));
            }
        }
    }

    /* sprites projectiles */
    static const char *proj_noms[NB_ARMES] = {
        "proj_shuriken", "proj_fleche", "proj_kunai", "proj_magie"
    };
    for (f = 0; f < NB_ARMES; f++) {
        sprintf(chemin, "sprites/joueur/%s.pcx", proj_noms[f]);
        j->sprites_proj[f] = load_bitmap(chemin, NULL);
        if (j->sprites_proj[f] == NULL) {
            j->sprites_proj[f] = create_bitmap(8, 8);
            clear_to_color(j->sprites_proj[f], makecol(192, 57, 43));
        }
    }

    return 1;
}

void liberer_joueur(Joueur *j)
{
    int e, f;
    if (j == NULL) return;

    for (e = 0; e < NB_ETATS_ANIM; e++) {
        if (j->frames[e] == NULL) continue;
        for (f = 0; f < j->nb_frames[e]; f++) {
            if (j->frames[e][f]) destroy_bitmap(j->frames[e][f]);
        }
        free(j->frames[e]);
        j->frames[e] = NULL;
    }

    for (f = 0; f < NB_ARMES; f++) {
        if (j->sprites_proj[f]) destroy_bitmap(j->sprites_proj[f]);
    }
}


/* =========================================================
   UPDATE PRINCIPAL
   ========================================================= */

void update_joueur(Joueur *j, NiveauConfig *niveau)
{
    if (j == NULL || j->mort) return;

    /* ----- cooldowns ----- */
    if (j->cooldown_tir    > 0) j->cooldown_tir--;
    if (j->invincible      > 0) j->invincible--;
    if (j->dash_cooldown   > 0) j->dash_cooldown--;
    if (j->saut_buffer     > 0) j->saut_buffer--;
    if (j->coyote_timer    > 0) j->coyote_timer--;

    /* ----- dash en cours ----- */
    if (j->dash_actif) {
        j->dash_timer--;
        if (j->dash_timer <= 0) {
            j->dash_actif = 0;
            j->vx = 0.0f;
        }
    }

    /* ----- lecture clavier ----- */
    if (!j->dash_actif) {
        joueur_deplacer(j, niveau);
    }

    /* ----- saut ----- */
    if (key[KEY_Z] || key[KEY_UP]) {
        if (j->saut_buffer == 0)        /* évite la répétition */
            j->saut_buffer = JOUEUR_SAUT_BUFFER;
    }
    if (j->saut_buffer > 0 &&
        (j->au_sol || j->coyote_timer > 0 || j->nb_sauts_restants > 0)) {
        joueur_sauter(j);
        j->saut_buffer = 0;
    }

    /* ----- dash (shift) ----- */
    if ((key[KEY_LSHIFT] || key[KEY_RSHIFT]) &&
        j->dash_cooldown == 0 && !j->dash_actif) {
        joueur_dash(j);
    }

    /* ----- tir ----- */
    if (key[KEY_X] || key[KEY_SPACE]) {
        joueur_tirer(j);
    }

    /* ----- changement d'arme (1-4) ----- */
    if (key[KEY_1]) joueur_changer_arme(j, ARME_SHURIKEN);
    if (key[KEY_2]) joueur_changer_arme(j, ARME_ARC);
    if (key[KEY_3]) joueur_changer_arme(j, ARME_KUNAI);
    if (key[KEY_4]) joueur_changer_arme(j, ARME_MAGIE);

    /* ----- physique ----- */
    joueur_appliquer_gravite(j);
    joueur_resoudre_collisions(j, niveau);

    /* ----- projectiles ----- */
    update_projectiles_joueur(j, niveau);

    /* ----- animation ----- */
    update_animation_joueur(j);

    /* ----- rechargement mana passif (1 pt / 90 frames) ----- */
    /* géré par jeu.c via joueur_recharger_mana si besoin */
}


/* =========================================================
   MOUVEMENTS
   ========================================================= */

void joueur_deplacer(Joueur *j, NiveauConfig *niveau)
{
    (void)niveau;   /* réservé pour vérifications futures */

    int gauche  = key[KEY_Q] || key[KEY_LEFT];
    int droite  = key[KEY_D] || key[KEY_RIGHT];

    if (droite) {
        j->vx  = JOUEUR_VITESSE;
        j->dir = DIR_DROITE;
    } else if (gauche) {
        j->vx  = -JOUEUR_VITESSE;
        j->dir = DIR_GAUCHE;
    } else {
        /* friction au sol */
        if (j->au_sol) {
            j->vx *= 0.7f;
            if (j->vx > -0.1f && j->vx < 0.1f) j->vx = 0.0f;
        } else {
            j->vx *= 0.92f;   /* résistance aérienne légère */
        }
    }

    /* déplacement horizontal */
    j->x += j->vx;

    /* bornes écran */
    if (j->x < 0)                             j->x = 0;
    if (j->x > niveau->largeur - j->largeur)  j->x = (float)(niveau->largeur - j->largeur);
}

void joueur_sauter(Joueur *j)
{
    if (j->nb_sauts_restants <= 0) return;

    j->vy = JOUEUR_SAUT_FORCE;
    j->nb_sauts_restants--;
    j->au_sol      = 0;
    j->coyote_timer = 0;

    /* saut plus court si on relâche tôt → géré en update_physique */
}

void joueur_dash(Joueur *j)
{
    j->dash_actif    = 1;
    j->dash_timer    = JOUEUR_DASH_DUREE;
    j->dash_cooldown = JOUEUR_DASH_COOLDOWN;
    j->dash_dir      = j->dir;
    j->vx            = JOUEUR_VITESSE_DASH * (float)j->dir;
    j->vy            = 0.0f;   /* annule gravité pendant le dash */

    j->etat_anim = ANIM_DASH;
}

void joueur_appliquer_gravite(Joueur *j)
{
    if (j->dash_actif) return;   /* pas de gravité pendant le dash */

    /* saut variable : on coupe la vitesse verticale si touche relâchée */
    if (j->vy < 0.0f &&
        !(key[KEY_Z] || key[KEY_UP])) {
        j->vy += JOUEUR_GRAVITE * 2.0f;   /* chute plus rapide = saut court */
    } else {
        j->vy += JOUEUR_GRAVITE;
    }

    if (j->vy > JOUEUR_GRAVITE_MAX) j->vy = JOUEUR_GRAVITE_MAX;
}

void joueur_resoudre_collisions(Joueur *j, NiveauConfig *niveau)
{
    int etait_au_sol = j->au_sol;

    /* déplacement vertical */
    j->y     += j->vy;
    j->au_sol = 0;

    /* sol simple : bord bas de l'écran / tuiles */
    int sol = niveau_trouver_sol(niveau,
                                 (int)j->x + j->largeur / 2,
                                 (int)j->y + j->hauteur,
                                 (int)j->vy + 1);
    if (sol >= 0 && j->vy >= 0.0f) {
        j->y      = (float)(sol - j->hauteur);
        j->vy     = 0.0f;
        j->au_sol = 1;
        j->nb_sauts_restants = 2;
    }

    /* plafond */
    int plafond = niveau_trouver_plafond(niveau,
                                         (int)j->x + j->largeur / 2,
                                         (int)j->y,
                                         (int)j->vy);
    if (plafond >= 0 && j->vy < 0.0f) {
        j->y  = (float)(plafond);
        j->vy = 0.0f;
    }

    /* coyote time : on vient de quitter le bord */
    if (etait_au_sol && !j->au_sol && j->vy >= 0.0f) {
        j->coyote_timer = JOUEUR_COYOTE_FRAMES;
    }
}


/* =========================================================
   ARMES & TIR
   ========================================================= */

void joueur_changer_arme(Joueur *j, TypeArme arme)
{
    j->arme        = arme;
    j->cooldown_tir = 0;   /* reset cooldown au changement */
}

void joueur_tirer(Joueur *j)
{
    if (j->cooldown_tir > 0) return;

    /* la magie consomme du mana */
    if (j->arme == ARME_MAGIE) {
        if (j->mana < 15) return;   /* pas assez de mana */
        joueur_depenser_mana(j, 15);
    }

    ProjetileJoueur *p = trouver_slot_proj(j);
    if (p == NULL) return;

    memset(p, 0, sizeof(ProjetileJoueur));

    /* position de départ : centre du joueur + décalage */
    float cx = j->x + j->largeur  / 2.0f;
    float cy = j->y + j->hauteur  / 2.0f - 4.0f;
    float dx = (float)j->dir;

    p->actif = 1;
    p->dir   = j->dir;
    p->x     = cx + dx * (j->largeur / 2.0f + 2.0f);
    p->y     = cy;

    switch (j->arme) {

        case ARME_SHURIKEN:
            p->type      = PROJ_SHURIKEN;
            p->vx        = VITESSE_SHURIKEN * dx;
            p->vy        = 0.0f;
            p->degats    = DEGATS_SHURIKEN;
            p->frame_vie = 40;
            j->cooldown_tir = CD_SHURIKEN;
            j->etat_anim    = ANIM_ATTAQUE;
            j->frame_anim   = 0;
            break;

        case ARME_ARC:
            p->type      = PROJ_FLECHE;
            p->vx        = VITESSE_FLECHE * dx;
            /* légère trajectoire si on vise en haut/bas */
            p->vy        = (key[KEY_UP] || key[KEY_Z])   ? -2.5f :
                           (key[KEY_DOWN] || key[KEY_S]) ?  2.5f : 0.0f;
            p->degats    = DEGATS_FLECHE;
            p->frame_vie = 60;
            j->cooldown_tir = CD_ARC;
            j->etat_anim    = ANIM_ATTAQUE;
            j->frame_anim   = 0;
            break;

        case ARME_KUNAI:
            p->type      = PROJ_KUNAI;
            p->vx        = VITESSE_KUNAI * dx;
            p->vy        = -1.0f;   /* léger arc vers le haut */
            p->degats    = DEGATS_KUNAI;
            p->frame_vie = 35;
            j->cooldown_tir = CD_KUNAI;
            break;

        case ARME_MAGIE: {
            /* boule de feu avec légère sinusoïde (géré en update) */
            p->type      = PROJ_MAGIE;
            p->vx        = VITESSE_MAGIE * dx;
            p->vy        = 0.0f;
            p->degats    = DEGATS_MAGIE;
            p->frame_vie = 80;
            j->cooldown_tir = CD_MAGIE;
            j->etat_anim    = ANIM_ATTAQUE;
            j->frame_anim   = 0;
            break;
        }
    }

    p->sprite = j->sprites_proj[(int)j->arme];
}

void joueur_attaque_corps(Joueur *j)
{
    /*
       Crée une hitbox temporaire devant le joueur.
       Le vrai test de collision est fait dans collisions.c
       via joueur_hitbox_attaque_x/y/w/h.
       Ici on déclenche juste l'animation.
    */
    if (j->cooldown_tir > 0) return;
    j->etat_anim    = ANIM_ATTAQUE;
    j->frame_anim   = 0;
    j->cooldown_tir = CD_SHURIKEN;
}


/* =========================================================
   PROJECTILES
   ========================================================= */

ProjetileJoueur *trouver_slot_proj(Joueur *j)
{
    int i;
    for (i = 0; i < MAX_PROJ_JOUEUR; i++) {
        if (!j->projectiles[i].actif)
            return &j->projectiles[i];
    }
    return NULL;   /* pool plein */
}

void update_projectiles_joueur(Joueur *j, NiveauConfig *niveau)
{
    int i;
    for (i = 0; i < MAX_PROJ_JOUEUR; i++) {
        ProjetileJoueur *p = &j->projectiles[i];
        if (!p->actif) continue;

        /* durée de vie */
        p->frame_vie--;
        if (p->frame_vie <= 0) { p->actif = 0; continue; }

        p->frame_anim++;

        /* mouvement de base */
        p->x += p->vx;
        p->y += p->vy;

        /* mouvement sinusoïdal pour la magie — table 16 valeurs, sans math.h */
        if (p->type == PROJ_MAGIE) {
            static const float sinus[16] = {
                 0.00f,  0.38f,  0.71f,  0.92f,
                 1.00f,  0.92f,  0.71f,  0.38f,
                 0.00f, -0.38f, -0.71f, -0.92f,
                -1.00f, -0.92f, -0.71f, -0.38f
            };
            p->y += sinus[p->frame_anim & 15] * 1.2f;
        }

        /* gravité légère pour les kunais */
        if (p->type == PROJ_KUNAI) {
            p->vy += 0.18f;
        }

        /* collision avec les tuiles solides */
        if (niveau_est_solide(niveau, (int)p->x, (int)p->y)) {
            p->actif = 0;
            continue;
        }

        /* sortie de l'écran */
        if (p->x < -32 || p->x > niveau->largeur + 32 ||
            p->y < -32 || p->y > niveau->hauteur + 32) {
            p->actif = 0;
        }
    }
}

void draw_projectiles_joueur(BITMAP *buffer, Joueur *j)
{
    int i;
    for (i = 0; i < MAX_PROJ_JOUEUR; i++) {
        ProjetileJoueur *p = &j->projectiles[i];
        if (!p->actif || p->sprite == NULL) continue;

        int px = (int)p->x - p->sprite->w / 2;
        int py = (int)p->y - p->sprite->h / 2;

        /* shuriken : rotation simulée par flip alterné */
        if (p->type == PROJ_SHURIKEN && (p->frame_anim / 3) % 2 == 1) {
            draw_sprite_h_flip(buffer, p->sprite, px, py);
        } else {
            draw_sprite(buffer, p->sprite, px, py);
        }

        /* halo magie */
        if (p->type == PROJ_MAGIE) {
            int cx = (int)p->x;
            int cy = (int)p->y;
            int r  = 6 + (p->frame_anim % 4);
            circle(buffer, cx, cy, r,
                   makecol(192 + (p->frame_anim % 30), 57, 43));
        }
    }
}


/* =========================================================
   DÉGÂTS & VIE
   ========================================================= */

void joueur_prendre_degats(Joueur *j, int degats)
{
    if (j->invincible > 0 || j->mort) return;

    j->vie -= degats;
    j->invincible = JOUEUR_INVINCIBLE_DUR;

    /* recul */
    j->vx = -3.0f * (float)j->dir;
    j->vy = -2.5f;

    j->etat_anim  = ANIM_TOUCHE;
    j->frame_anim = 0;

    if (j->vie <= 0) {
        j->vie       = 0;
        j->mort      = 1;
        j->etat_anim = ANIM_MORT;
        j->frame_anim = 0;
    }
}

void joueur_soigner(Joueur *j, int soin)
{
    j->vie += soin;
    if (j->vie > j->vie_max) j->vie = j->vie_max;
}

void joueur_depenser_mana(Joueur *j, int cout)
{
    j->mana -= cout;
    if (j->mana < 0) j->mana = 0;
}

void joueur_recharger_mana(Joueur *j, int quantite)
{
    j->mana += quantite;
    if (j->mana > j->mana_max) j->mana = j->mana_max;
}


/* =========================================================
   ANIMATION
   ========================================================= */

/*  Priorité des états d'animation :
    mort > touché > attaque > dash > saut > chute > marche > idle  */
static EtatAnimation choisir_etat_anim(const Joueur *j)
{
    if (j->mort)             return ANIM_MORT;
    if (j->etat_anim == ANIM_TOUCHE  && j->frame_anim < j->nb_frames[ANIM_TOUCHE]  * 4) return ANIM_TOUCHE;
    if (j->etat_anim == ANIM_ATTAQUE && j->frame_anim < j->nb_frames[ANIM_ATTAQUE] * 4) return ANIM_ATTAQUE;
    if (j->dash_actif)       return ANIM_DASH;
    if (!j->au_sol && j->vy < 0.0f) return ANIM_SAUT;
    if (!j->au_sol && j->vy > 0.5f) return ANIM_CHUTE;
    if (j->vx != 0.0f)      return ANIM_MARCHE;
    return ANIM_IDLE;
}

void update_animation_joueur(Joueur *j)
{
    EtatAnimation nouvel_etat = choisir_etat_anim(j);

    if (nouvel_etat != j->etat_anim) {
        /* reset compteur sauf si l'état actuel doit finir */
        if (j->etat_anim != ANIM_ATTAQUE && j->etat_anim != ANIM_TOUCHE) {
            j->frame_anim = 0;
            j->anim_timer = 0;
        }
        j->etat_anim = nouvel_etat;
    }

    /* vitesse d'animation selon l'état */
    int vitesse = 6;   /* frames Allegro par frame sprite */
    if (j->etat_anim == ANIM_MARCHE)  vitesse = 5;
    if (j->etat_anim == ANIM_ATTAQUE) vitesse = 3;
    if (j->etat_anim == ANIM_MORT)    vitesse = 8;

    j->anim_timer++;
    if (j->anim_timer >= vitesse) {
        j->anim_timer = 0;

        int nb = j->nb_frames[j->etat_anim];

        /* mort : s'arrête sur la dernière frame */
        if (j->etat_anim == ANIM_MORT) {
            if (j->frame_anim < nb - 1) j->frame_anim++;
        } else {
            j->frame_anim = (j->frame_anim + 1) % nb;
        }
    }
}

void draw_joueur(BITMAP *buffer, Joueur *j)
{
    if (buffer == NULL || j == NULL) return;

    BITMAP *frame = NULL;
    int     idx   = j->frame_anim;
    int     nb    = j->nb_frames[j->etat_anim];

    if (j->frames[j->etat_anim] && nb > 0) {
        if (idx >= nb) idx = nb - 1;
        frame = j->frames[j->etat_anim][idx];
    }

    int px = (int)j->x;
    int py = (int)j->y;

    if (frame != NULL) {
        /* clignotement lors de l'invincibilité */
        int visible = 1;
        if (j->invincible > 0 && (j->invincible / 4) % 2 == 0) visible = 0;

        if (visible) {
            if (j->dir == DIR_GAUCHE) {
                draw_sprite_h_flip(buffer, frame, px, py);
            } else {
                draw_sprite(buffer, frame, px, py);
            }
        }
    } else {
        /* fallback : rectangle de débogage */
        int col = (j->invincible > 0)
                  ? makecol(192, 57, 43)
                  : makecol(201, 168, 76);
        rectfill(buffer, px, py,
                 px + j->largeur - 1,
                 py + j->hauteur - 1, col);
        rect(buffer, px, py,
             px + j->largeur - 1,
             py + j->hauteur - 1,
             makecol(245, 239, 224));
    }

    /* dessin des projectiles */
    draw_projectiles_joueur(buffer, j);
}


/* =========================================================
   HUD — Barre de vie, mana, arme
   ========================================================= */

static void draw_barre(BITMAP *buf, int x, int y, int w, int h,
                       int val, int max_val,
                       int col_fond, int col_plein)
{
    /* fond */
    rectfill(buf, x - 1, y - 1, x + w, y + h, COL_OMBRE);
    rectfill(buf, x, y, x + w - 1, y + h - 1, col_fond);

    /* remplissage proportionnel */
    if (max_val > 0 && val > 0) {
        int rempli = (val * w) / max_val;
        if (rempli < 1) rempli = 1;
        rectfill(buf, x, y, x + rempli - 1, y + h - 1, col_plein);
    }

    /* bordure */
    rect(buf, x - 1, y - 1, x + w, y + h, makecol(80, 60, 40));
}

static const char *nom_arme(TypeArme a)
{
    switch (a) {
        case ARME_SHURIKEN: return "SHURIKEN";
        case ARME_ARC:      return "ARC";
        case ARME_KUNAI:    return "KUNAI";
        case ARME_MAGIE:    return "MAGIE";
        default:            return "???";
    }
}

void draw_joueur_hud(BITMAP *buffer, Joueur *j)
{
    if (buffer == NULL || j == NULL) return;

    int x0 = 12, y0 = 12;

    /* --- vie --- */
    textprintf_ex(buffer, font, x0, y0,
                  COL_OR, -1, "VIE");
    draw_barre(buffer, x0 + 28, y0, 100, 8,
               j->vie, j->vie_max,
               COL_VIE_FOND, COL_VIE_PLEIN);

    /* --- mana --- */
    textprintf_ex(buffer, font, x0, y0 + 14,
                  COL_OR, -1, "KI ");
    draw_barre(buffer, x0 + 28, y0 + 14, 100, 8,
               j->mana, j->mana_max,
               COL_MANA_FOND, COL_MANA_PLEIN);

    /* --- arme courante --- */
    textprintf_ex(buffer, font, x0, y0 + 30,
                  COL_BLANC, -1,
                  "[%s]  %d/%d PV",
                  nom_arme(j->arme),
                  j->vie, j->vie_max);

    /* --- indicateur cooldown tir --- */
    if (j->cooldown_tir > 0) {
        int cd_max = (j->arme == ARME_MAGIE) ? CD_MAGIE :
                     (j->arme == ARME_ARC)   ? CD_ARC   :
                     (j->arme == ARME_KUNAI)  ? CD_KUNAI : CD_SHURIKEN;
        draw_barre(buffer, x0 + 28, y0 + 42, 60, 4,
                   cd_max - j->cooldown_tir, cd_max,
                   makecol(40, 30, 20), COL_OR);
    }
}


/* =========================================================
   UTILITAIRES
   ========================================================= */

int joueur_hitbox_x(const Joueur *j) { return (int)j->x; }
int joueur_hitbox_y(const Joueur *j) { return (int)j->y; }

int joueur_centre_x(const Joueur *j)
{
    return (int)j->x + j->largeur / 2;
}

int joueur_centre_y(const Joueur *j)
{
    return (int)j->y + j->hauteur / 2;
}

int joueur_est_vivant(const Joueur *j)
{
    return (j != NULL && !j->mort && j->vie > 0);
}