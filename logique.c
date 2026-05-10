#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "logique.h"

#define FICHIER_SAVE "sauvegardes.txt"

/* ── Utilitaires sans math.h ─────────────────────────── */
static float abs_f(float v) { return v < 0.0f ? -v : v; }

/* sinus approché (table 16 valeurs) */
static float sin_f(int t)
{
    static const float tbl[16] = {
         0.00f,  0.38f,  0.71f,  0.92f,
         1.00f,  0.92f,  0.71f,  0.38f,
         0.00f, -0.38f, -0.71f, -0.92f,
        -1.00f, -0.92f, -0.71f, -0.38f
    };
    return tbl[t & 15];
}

static float bulle_rayon(int taille)
{
    return (float)(BULLE_RAYON_BASE * taille);
}

/* ── Générateur pseudo-aléatoire interne ─────────────── */
static unsigned int graine = 12345;
static int rand_local(void)
{
    graine = graine * 1664525u + 1013904223u;
    return (int)((graine >> 16) & 0x7FFF);
}

/* =========================================================
   JOUEUR
   ========================================================= */

void logique_init_joueur(Joueur *j, const char *pseudo)
{
    j->x                = WINDOW_W / 2.0f;
    j->y                = GAME_H - PLAYER_H / 2.0f;
    j->vx               = 0;
    j->arme             = ARME_SIMPLE;
    j->arme_timer       = 0;
    j->invincible       = false;
    j->invincible_timer = 0;
    j->vivant           = true;
    j->score            = 0;
    strncpy(j->pseudo, pseudo, 31);
    j->pseudo[31] = '\0';
}

void logique_deplacer_joueur(Joueur *j, bool gauche, bool droite)
{
    if      (gauche) j->vx = -PLAYER_SPEED;
    else if (droite) j->vx =  PLAYER_SPEED;
    else             j->vx =  0;
}

void logique_update_joueur(Joueur *j)
{
    float demi = PLAYER_W / 2.0f;
    j->x += j->vx;
    if (j->x - demi < 0)        j->x = demi;
    if (j->x + demi > WINDOW_W) j->x = WINDOW_W - demi;

    if (j->arme_timer > 0) {
        j->arme_timer--;
        if (j->arme_timer == 0) j->arme = ARME_SIMPLE;
    }
    if (j->invincible_timer > 0) {
        j->invincible_timer--;
        if (j->invincible_timer == 0) j->invincible = false;
    }
}

/* =========================================================
   PROJECTILES
   ========================================================= */

Projectile *logique_creer_projectile(float x, float y, TypeArme arme)
{
    Projectile *p = (Projectile *)malloc(sizeof(Projectile));
    p->x       = x;
    p->y       = y;
    p->vy      = -(float)PROJ_SPEED;
    p->actif   = true;
    p->type    = arme;
    p->suivant = NULL;
    return p;
}

bool logique_tirer(Joueur *j, Projectile **liste, int *cooldown)
{
    float cx, cy;
    if (*cooldown > 0) return false;
    cx = j->x;
    cy = j->y - PLAYER_H / 2.0f;

    switch (j->arme) {
        case ARME_SIMPLE:
            *liste = logique_creer_projectile(cx, cy, ARME_SIMPLE);
            *cooldown = 18;
            break;
        case ARME_DOUBLE: {
            Projectile *p1 = logique_creer_projectile(cx - 12, cy, ARME_DOUBLE);
            Projectile *p2 = logique_creer_projectile(cx + 12, cy, ARME_DOUBLE);
            p1->suivant = p2;
            *liste = p1;
            *cooldown = 18;
            break;
        }
        case ARME_TRIPLE: {
            Projectile *p1 = logique_creer_projectile(cx - 16, cy, ARME_TRIPLE);
            Projectile *p2 = logique_creer_projectile(cx,      cy, ARME_TRIPLE);
            Projectile *p3 = logique_creer_projectile(cx + 16, cy, ARME_TRIPLE);
            p1->suivant = p2; p2->suivant = p3;
            *liste = p1;
            *cooldown = 15;
            break;
        }
        case ARME_ECLAIR: {
            Projectile *p = logique_creer_projectile(cx, cy, ARME_ECLAIR);
            p->vy  = -(float)(PROJ_SPEED + 4);
            *liste = p;
            *cooldown = 25;
            break;
        }
    }
    return true;
}

void logique_update_projectiles(Projectile **liste)
{
    Projectile **cur = liste;
    while (*cur) {
        Projectile *p = *cur;
        if (p->actif) {
            p->y += p->vy;
            if (p->y + PROJ_H < 0) p->actif = false;
        }
        if (!p->actif) { *cur = p->suivant; free(p); }
        else            cur   = &p->suivant;
    }
}

void logique_liberer_projectiles(Projectile **liste)
{
    while (*liste) {
        Projectile *tmp = *liste;
        *liste = tmp->suivant;
        free(tmp);
    }
}

/* =========================================================
   BULLES
   ========================================================= */

Bulle *logique_creer_bulle(float x, float y, int taille, bool eclairs)
{
    Bulle *b      = (Bulle *)malloc(sizeof(Bulle));
    float vx_base = 2.0f + taille * 0.5f;
    b->x            = x;
    b->y            = y;
    b->vx           = (rand_local() % 2 == 0) ? vx_base : -vx_base;
    b->vy           = BOUNCE_VY_BASE - taille * 1.2f;
    b->taille       = taille;
    b->lance_eclairs = eclairs;
    b->eclair_timer = 80 + rand_local() % 120;
    b->active       = true;
    b->suivante     = NULL;
    return b;
}

void logique_update_bulles(Bulle **liste, int niveau)
{
    Bulle *b = *liste;
    (void)niveau;
    while (b) {
        float r, sol;
        if (!b->active) { b = b->suivante; continue; }

        b->vy += GRAVITY;
        b->x  += b->vx;
        b->y  += b->vy;
        r   = bulle_rayon(b->taille);
        sol = (float)GAME_H - r;

        if (b->x - r < 0) {
            b->x = r; b->vx = abs_f(b->vx);
            b->choc_timer = 8;
        }
        if (b->x + r > WINDOW_W) {
            b->x = WINDOW_W - r; b->vx = -abs_f(b->vx);
            b->choc_timer = 8;
        }
        if (b->y >= sol) {
            b->y  = sol;
            b->vy = BOUNCE_VY_BASE - b->taille * 1.2f;
            b->vx = (b->vx > 0) ? abs_f(b->vx) : -abs_f(b->vx);
            b->choc_timer = 8;
        }
        if (b->y - r < GAME_Y) {
            b->y = (float)GAME_Y + r;
            if (b->vy < 0) b->vy = -b->vy * 0.5f;
            b->choc_timer = 8;
        }
        if (b->choc_timer > 0) b->choc_timer--;
        if (b->lance_eclairs && b->eclair_timer > 0)
            b->eclair_timer--;

        b = b->suivante;
    }
}

void logique_liberer_bulles(Bulle **liste)
{
    while (*liste) {
        Bulle *tmp = *liste;
        *liste = tmp->suivante;
        free(tmp);
    }
}

int logique_compter_bulles(Bulle *liste)
{
    int n = 0;
    while (liste) { if (liste->active) n++; liste = liste->suivante; }
    return n;
}

static TypeBonus choisir_bonus(int taille)
{
    int r = rand_local() % 10;
    if (taille >= 3) {
        if (r < 3) return BONUS_ARME_DOUBLE;
        if (r < 5) return BONUS_ARME_TRIPLE;
        if (r < 6) return BONUS_ARME_ECLAIR;
        if (r < 7) return BONUS_TEMPS;
    } else {
        if (r < 2) return BONUS_INVINCIBLE;
        if (r < 4) return BONUS_ARME_DOUBLE;
    }
    return (TypeBonus)-1;
}

int logique_diviser_bulle(Bulle **liste, Bulle *cible,
                           BonusItem **bonus_liste, int niveau)
{
    int score = 0;
    cible->active = false;

    switch (cible->taille) {
        case 1: score = SCORE_BULLE_1; break;
        case 2: score = SCORE_BULLE_2; break;
        case 3: score = SCORE_BULLE_3; break;
        case 4: score = SCORE_BULLE_4; break;
    }

    if (cible->taille > 1) {
        int   nt = cible->taille - 1;
        bool  ec = (niveau >= 3) && (rand_local() % 4 == 0);
        Bulle *b1 = logique_creer_bulle(cible->x, cible->y, nt, ec);
        Bulle *b2 = logique_creer_bulle(cible->x, cible->y, nt, ec);
        b1->vx =  abs_f(b1->vx);
        b2->vx = -abs_f(b2->vx);
        b1->vy = b2->vy = BOUNCE_VY_BASE - nt;
        b1->suivante = *liste; *liste = b1;
        b2->suivante = *liste; *liste = b2;
    }

    if (niveau >= 2) {
        TypeBonus type = choisir_bonus(cible->taille);
        if ((int)type >= 0) {
            BonusItem *item = logique_creer_bonus(cible->x, cible->y, type);
            item->suivant   = *bonus_liste;
            *bonus_liste    = item;
        }
    }
    return score;
}

/* =========================================================
   ÉCLAIRS
   ========================================================= */

Eclair *logique_creer_eclair(float x, float y)
{
    Eclair *e  = (Eclair *)malloc(sizeof(Eclair));
    e->x       = x;
    e->y       = y;
    e->vy      = 6.0f;
    e->actif   = true;
    e->suivant = NULL;
    return e;
}

void logique_update_eclairs(Eclair **liste)
{
    Eclair **cur = liste;
    while (*cur) {
        Eclair *e = *cur;
        if (e->actif) { e->y += e->vy; if (e->y > GAME_H) e->actif = false; }
        if (!e->actif) { *cur = e->suivant; free(e); }
        else            cur   = &e->suivant;
    }
}

void logique_liberer_eclairs(Eclair **liste)
{
    while (*liste) {
        Eclair *tmp = *liste;
        *liste = tmp->suivant;
        free(tmp);
    }
}

/* =========================================================
   BONUS
   ========================================================= */

BonusItem *logique_creer_bonus(float x, float y, TypeBonus type)
{
    BonusItem *b = (BonusItem *)malloc(sizeof(BonusItem));
    b->x        = x; b->y = y;
    b->vy       = 0.0f;        /* gravité démarre à 0 */
    b->type     = type;
    b->actif    = true;
    b->duree_vie = 8 * FPS;
    b->suivant  = NULL;
    return b;
}

void logique_update_bonus(BonusItem **liste)
{
    float sol = (float)GAME_H - 20.0f;
    BonusItem **cur = liste;
    while (*cur) {
        BonusItem *b = *cur;
        if (b->actif) {
            /* gravité */
            b->vy  += 0.3f;
            b->y   += b->vy;
            /* sol : rebond amorti */
            if (b->y >= sol) {
                b->y  = sol;
                b->vy = -b->vy * 0.4f;
                if (b->vy > -0.5f) b->vy = 0.0f;
            }
            b->duree_vie--;
            if (b->duree_vie <= 0) b->actif = false;
        }
        if (!b->actif) { *cur = b->suivant; free(b); }
        else            cur   = &b->suivant;
    }
}

void logique_liberer_bonus(BonusItem **liste)
{
    while (*liste) {
        BonusItem *tmp = *liste;
        *liste = tmp->suivant;
        free(tmp);
    }
}

void logique_appliquer_bonus(Joueur *j, TypeBonus type, EtatNiveau *niveau)
{
    switch (type) {
        case BONUS_ARME_DOUBLE:  j->arme = ARME_DOUBLE; j->arme_timer = 15*FPS; break;
        case BONUS_ARME_TRIPLE:  j->arme = ARME_TRIPLE; j->arme_timer = 12*FPS; break;
        case BONUS_ARME_ECLAIR:  j->arme = ARME_ECLAIR; j->arme_timer = 10*FPS; break;
        case BONUS_TEMPS:        if (niveau) niveau->temps_restant += 15;        break;
        case BONUS_INVINCIBLE:   j->invincible = true; j->invincible_timer = 5*FPS; break;
        default: break;
    }
}

/* =========================================================
   BOSS
   ========================================================= */

void logique_init_boss(Boss *b)
{
    b->x           = WINDOW_W / 2.0f;
    b->y           = 120.0f;
    b->vx          = 2.5f;
    b->vy          = 0.0f;
    b->pv          = 20;
    b->pv_max      = 20;
    b->actif       = true;
    b->spawn_timer = 3 * FPS;
    b->hit_flash   = 0;
}

void logique_update_boss(Boss *b, Bulle **bulles)
{
    int tick;
    if (!b->actif) return;

    b->x += b->vx;
    /* déplacement vertical sinusoïdal sans math.h */
    tick  = (int)(b->x / 4.0f) & 15;
    b->y  = 120.0f + sin_f(tick) * 50.0f;

    if (b->x < 60 || b->x > WINDOW_W - 60) b->vx = -b->vx;

    if (b->spawn_timer > 0) b->spawn_timer--;
    if (b->spawn_timer == 0) {
        Bulle *nouvelle  = logique_creer_bulle(b->x, b->y + 40, 2, false);
        nouvelle->suivante = *bulles;
        *bulles            = nouvelle;
        b->spawn_timer     = 4 * FPS;
    }
    if (b->hit_flash > 0) b->hit_flash--;
}

bool logique_hit_boss(Boss *b)
{
    float accel;
    if (!b->actif) return false;
    b->pv--;
    b->hit_flash = 10;
    accel = 1.0f + (float)(b->pv_max - b->pv) * 0.12f;
    b->vx = (b->vx > 0 ? 1.0f : -1.0f) * (2.5f * accel);
    if (b->pv <= 0) { b->actif = false; return true; }
    return false;
}

/* =========================================================
   COLLISIONS
   ========================================================= */

bool logique_collision_cercle_rect(float cx, float cy, float r,
                                   float rx, float ry, float rw, float rh)
{
    float px = cx < rx ? rx : (cx > rx+rw ? rx+rw : cx);
    float py = cy < ry ? ry : (cy > ry+rh ? ry+rh : cy);
    float dx = cx - px, dy = cy - py;
    return (dx*dx + dy*dy) < (r*r);
}

bool logique_collision_rect_rect(float ax, float ay, float aw, float ah,
                                 float bx, float by, float bw, float bh)
{
    return ax < bx+bw && ax+aw > bx && ay < by+bh && ay+ah > by;
}

bool logique_tester_collisions(Joueur *j, Projectile **projs,
                                Bulle **bulles, Eclair **eclairs,
                                BonusItem **bonus, Boss *boss,
                                int *score_gagne, int niveau)
{
    float jx = j->x - PLAYER_W/2.0f;
    float jy = j->y - PLAYER_H;
    float jw = (float)PLAYER_W;
    float jh = (float)PLAYER_H;
    Projectile *p;
    Bulle      *b;
    Eclair     *e;
    BonusItem  *bon;

    *score_gagne = 0;

    /* Projectiles vs Bulles */
    p = *projs;
    while (p) {
        if (p->actif) {
            b = *bulles;
            while (b) {
                if (b->active) {
                    float r = bulle_rayon(b->taille);
                    if (logique_collision_cercle_rect(b->x, b->y, r,
                            p->x, p->y, (float)PROJ_W, (float)PROJ_H)) {
                        p->actif = false;
                        *score_gagne += logique_diviser_bulle(bulles, b,
                                                               bonus, niveau);
                        break;
                    }
                }
                b = b->suivante;
            }
        }
        p = p->suivant;
    }

    /* Projectiles vs Boss */
    if (boss && boss->actif) {
        float bx = boss->x - 50, by = boss->y - 50;
        p = *projs;
        while (p) {
            if (p->actif &&
                logique_collision_rect_rect(p->x, p->y,
                    (float)PROJ_W, (float)PROJ_H, bx, by, 100, 100)) {
                p->actif = false;
                *score_gagne += 500;
                if (logique_hit_boss(boss)) *score_gagne += 5000;
            }
            p = p->suivant;
        }
    }

    if (j->invincible) return false;

    /* Bulles vs Joueur */
    b = *bulles;
    while (b) {
        if (b->active) {
            float r = bulle_rayon(b->taille);
            if (logique_collision_cercle_rect(b->x, b->y, r,
                    jx, jy, jw, jh)) {
                j->vivant = false; return true;
            }
        }
        b = b->suivante;
    }

    /* Éclairs vs Joueur */
    e = *eclairs;
    while (e) {
        if (e->actif &&
            logique_collision_rect_rect(e->x-4, e->y, 8, 20,
                jx, jy, jw, jh)) {
            j->vivant = false; return true;
        }
        e = e->suivant;
    }

    /* Boss vs Joueur */
    if (boss && boss->actif) {
        float bx = boss->x-50, by = boss->y-50;
        if (logique_collision_rect_rect(jx, jy, jw, jh,
                bx, by, 100, 100)) {
            j->vivant = false; return true;
        }
    }

    /* Bonus vs Joueur */
    bon = *bonus;
    while (bon) {
        if (bon->actif &&
            logique_collision_rect_rect(bon->x-16, bon->y-16, 32, 32,
                jx, jy, jw, jh)) {
            logique_appliquer_bonus(j, bon->type, NULL);
            bon->actif = false;
        }
        bon = bon->suivant;
    }

    return false;
}

/* =========================================================
   NIVEAU
   ========================================================= */

void logique_init_niveau(EtatNiveau *n, int numero)
{
    n->numero        = numero;
    n->temps_restant = 90 - (numero - 1) * 5;
    if (n->temps_restant < 60) n->temps_restant = 60;
    n->frame_timer   = FPS;
    n->en_cours      = false;
    n->gagne         = false;
    n->countdown     = 3 * FPS + 30;
}

bool logique_update_niveau(EtatNiveau *n)
{
    if (n->temps_restant <= 0) return true;
    n->frame_timer--;
    if (n->frame_timer <= 0) {
        n->frame_timer = FPS;
        n->temps_restant--;
    }
    return false;
}

void logique_peupler_niveau(int numero, Bulle **bulles,
                             Boss *boss, Joueur *j)
{
    Bulle *b1, *b2, *b3, *b4, *b5;
    j->x      = WINDOW_W / 2.0f;
    j->y      = GAME_H - PLAYER_H / 2.0f;
    j->arme   = ARME_SIMPLE;
    j->arme_timer = 0;
    j->invincible = false;
    j->vivant = true;
    logique_liberer_bulles(bulles);

    switch (numero) {
        case 1:
            b1 = logique_creer_bulle(200, 200, 4, false);
            b2 = logique_creer_bulle(700, 150, 3, false);
            b1->suivante = b2; *bulles = b1;
            break;
        case 2:
            b1 = logique_creer_bulle(150, 180, 4, false);
            b2 = logique_creer_bulle(500, 200, 3, false);
            b3 = logique_creer_bulle(800, 160, 4, false);
            b1->suivante = b2; b2->suivante = b3; *bulles = b1;
            break;
        case 3:
            b1 = logique_creer_bulle(100, 150, 4, true);
            b2 = logique_creer_bulle(400, 200, 3, false);
            b3 = logique_creer_bulle(650, 170, 4, true);
            b4 = logique_creer_bulle(900, 130, 2, false);
            b1->suivante = b2; b2->suivante = b3;
            b3->suivante = b4; *bulles = b1;
            break;
        case 4:
            b1 = logique_creer_bulle(100, 120, 4, true);
            b2 = logique_creer_bulle(300, 180, 4, true);
            b3 = logique_creer_bulle(512, 100, 3, true);
            b4 = logique_creer_bulle(720, 160, 4, false);
            b5 = logique_creer_bulle(900, 140, 3, true);
            b1->suivante = b2; b2->suivante = b3;
            b3->suivante = b4; b4->suivante = b5; *bulles = b1;
            break;
        case 5:
            b1 = logique_creer_bulle(300, 200, 3, false);
            *bulles = b1;
            if (boss) logique_init_boss(boss);
            break;
        default:
            logique_peupler_niveau(1, bulles, NULL, j);
            break;
    }
}

/* =========================================================
   FIREBALL (projectile du boss)
   Normalisation sans math.h : alpha-max beta-min
   ========================================================= */

static float longueur_vec(float dx, float dy)
{
    float ax = dx < 0 ? -dx : dx;
    float ay = dy < 0 ? -dy : dy;
    float mn = ax < ay ? ax : ay;
    float mx = ax > ay ? ax : ay;
    return mx * 0.96f + mn * 0.40f;
}

void fireball_init_pool(Fireball *pool, int nb)
{
    int i;
    for (i = 0; i < nb; i++) {
        pool[i].actif      = false;
        pool[i].x          = 0;
        pool[i].y          = 0;
        pool[i].vx         = 0;
        pool[i].vy         = 0;
        pool[i].frame_anim = 0;
    }
}

void fireball_lancer(Fireball *pool, int nb,
                     float bx, float by,
                     float cible_x, float cible_y,
                     float vitesse)
{
    int   i;
    float dx, dy, len;

    for (i = 0; i < nb; i++) {
        if (!pool[i].actif) {
            dx  = cible_x - bx;
            dy  = cible_y - by;
            len = longueur_vec(dx, dy);
            if (len < 1.0f) len = 1.0f;

            pool[i].x          = bx;
            pool[i].y          = by;
            pool[i].vx         = (dx / len) * vitesse;
            pool[i].vy         = (dy / len) * vitesse;
            pool[i].actif      = true;
            pool[i].frame_anim = 0;
            return;   /* une seule fireball à la fois */
        }
    }
}

void fireball_update(Fireball *pool, int nb)
{
    int i;
    for (i = 0; i < nb; i++) {
        if (!pool[i].actif) continue;
        pool[i].x += pool[i].vx;
        pool[i].y += pool[i].vy;
        pool[i].frame_anim++;
        /* sortie écran */
        if (pool[i].x < -32 || pool[i].x > WINDOW_W + 32 ||
            pool[i].y < -32 || pool[i].y > WINDOW_H + 32)
            pool[i].actif = false;
    }
}

int fireball_touche_joueur(Fireball *pool, int nb, const Joueur *j)
{
    int   i;
    float jx = j->x - PLAYER_W / 2.0f;
    float jy = j->y - PLAYER_H;
    float jw = (float)PLAYER_W;
    float jh = (float)PLAYER_H;
    float r  = 12.0f;   /* rayon fireball */

    for (i = 0; i < nb; i++) {
        if (!pool[i].actif) continue;
        if (logique_collision_cercle_rect(pool[i].x, pool[i].y, r,
                                          jx, jy, jw, jh)) {
            pool[i].actif = false;
            return 1;
        }
    }
    return 0;
}

/* =========================================================
   SAUVEGARDE
   ========================================================= */

void logique_sauvegarder(const char *pseudo, int niveau,
                          int score, const char *fichier)
{
    Sauvegarde saves[MAX_SAUVEGARDES];
    int nb = 0, i;
    bool trouve = false;
    FILE *f = fopen(fichier, "r");
    if (f) {
        while (nb < MAX_SAUVEGARDES &&
               fscanf(f, "%31s %d %d",
                      saves[nb].pseudo,
                      &saves[nb].dernier_niveau,
                      &saves[nb].meilleur_score) == 3)
            nb++;
        fclose(f);
    }
    for (i = 0; i < nb; i++) {
        if (strcmp(saves[i].pseudo, pseudo) == 0) {
            if (niveau > saves[i].dernier_niveau)
                saves[i].dernier_niveau = niveau;
            if (score > saves[i].meilleur_score)
                saves[i].meilleur_score = score;
            trouve = true; break;
        }
    }
    if (!trouve && nb < MAX_SAUVEGARDES) {
        strncpy(saves[nb].pseudo, pseudo, 31);
        saves[nb].dernier_niveau = niveau;
        saves[nb].meilleur_score = score;
        nb++;
    }
    f = fopen(fichier, "w");
    if (f) {
        for (i = 0; i < nb; i++)
            fprintf(f, "%s %d %d\n",
                    saves[i].pseudo,
                    saves[i].dernier_niveau,
                    saves[i].meilleur_score);
        fclose(f);
    }
}

bool logique_charger(const char *pseudo, int *niveau,
                     int *score, const char *fichier)
{
    char buf[32];
    int  niv, sc;
    FILE *f = fopen(fichier, "r");
    if (!f) return false;
    while (fscanf(f, "%31s %d %d", buf, &niv, &sc) == 3) {
        if (strcmp(buf, pseudo) == 0) {
            *niveau = niv; *score = sc;
            fclose(f); return true;
        }
    }
    fclose(f); return false;
}