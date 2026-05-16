/* =========================================================
 *  graphique.c  —  Couche d'affichage Allegro 4
 *  Toutes les entites chargees depuis assets/
 *  Images manquantes : warning console, pas de popup bloquant
 * ========================================================= */

#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "graphismes.h"
#include "types.h"

/* =========================================================
   CHEMIN ASSETS
   ========================================================= */

static char s_asset_dir[256] = "assets";

static void asset_path(char *out, const char *nom)
{
    snprintf(out, 512, "%s/%s", s_asset_dir, nom);
}

/* =========================================================
   OUTILS IMAGES
   ========================================================= */

void graphique_nettoyer_magenta(BITMAP *bmp)
{
    int x, y;
    if (!bmp) return;
    for (y = 0; y < bmp->h; y++)
        for (x = 0; x < bmp->w; x++) {
            int c = getpixel(bmp, x, y);
            if (getr(c) > 150 && getb(c) > 150 && getg(c) < 120)
                putpixel(bmp, x, y, makecol(255, 0, 255));
        }
}

/* Charge un BMP — retourne NULL sans bloquer si absent */
static BITMAP *charger_bmp(const char *nom)
{
    char chemin[512];
    BITMAP *b;
    asset_path(chemin, nom);
    b = load_bitmap(chemin, NULL);
    if (!b) {
        fprintf(stderr, "[WARN] Image absente : %s\n", chemin);
        return NULL;
    }
    graphique_nettoyer_magenta(b);
    return b;
}

/* Dessine une image stretchee avec transparence magenta */
static void dessiner_image(BITMAP *dest, BITMAP *img,
                            int x, int y, int w, int h)
{
    if (!img) return;
    masked_stretch_blit(img, dest, 0, 0, img->w, img->h, x, y, w, h);
}

/* Dessine un fond (stretch plein ecran, pas de transparence) */
static void dessiner_fond(BITMAP *dest, BITMAP *img, int w, int h)
{
    if (!img) {
        clear_to_color(dest, makecol(0, 0, 0));
        return;
    }
    stretch_blit(img, dest, 0, 0, img->w, img->h, 0, 0, w, h);
}

/* =========================================================
   TEXTE STYLISE
   ========================================================= */

void graphique_textout_stylise(BITMAP *dest, const char *msg,
                                int x, int y, int taille,
                                int couleur, bool centre)
{
    int lw = text_length(font, msg);
    int lh = 8;
    int px = centre ? x - (lw * taille) / 2 : x;
    BITMAP *tmp = create_bitmap(lw, lh);
    if (!tmp) return;

    /* ombre */
    clear_to_color(tmp, makecol(255, 0, 255));
    textout_ex(tmp, font, msg, 0, 0, makecol(0, 0, 0), -1);
    stretch_sprite(dest, tmp, px + taille, y + taille,
                   lw * taille, lh * taille);

    /* texte */
    clear_to_color(tmp, makecol(255, 0, 255));
    textout_ex(tmp, font, msg, 0, 0, couleur, -1);
    stretch_sprite(dest, tmp, px, y, lw * taille, lh * taille);

    destroy_bitmap(tmp);
}

/* =========================================================
   INIT ALLEGRO
   ========================================================= */

void graphique_init_allegro(void)
{
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,
                     WINDOW_W, WINDOW_H, 0, 0) != 0) {
        allegro_message("Erreur fenetre : %s", allegro_error);
        exit(1);
    }
    set_window_title("Super Bulles");
}

/* =========================================================
   CHARGEMENT RESSOURCES
   ========================================================= */

bool graphique_charger_ressources(Ressources *res,
                                   const char *dossier)
{
    int i;
    const char *noms_niv[NB_NIVEAUX] = {
        "niveau1.bmp", "niveau2.bmp", "niveau3.bmp",
        "niveau4.bmp", "niveau5_boss.bmp"
    };

    strncpy(s_asset_dir, dossier, 255);
    s_asset_dir[255] = '\0';
    memset(res, 0, sizeof(Ressources));

    /* ── Fonds menus ─────────────────────────────────────── */
    res->fond_menu          = charger_bmp("fond.bmp");
    res->fond_jouer         = charger_bmp("fond_jouer.bmp");
    res->panneau_geant      = charger_bmp("panneau_geant.bmp");
    res->choix_joueurs_fond = charger_bmp("choix_joueurs_fond.bmp");
    res->saisie_pseudo_fond = charger_bmp("saisie_pseudo_fond.bmp");
    res->pause_fond         = charger_bmp("pause_fond.bmp");
    res->fin_niveau_fond    = charger_bmp("fin_niveau_fond.bmp");
    res->victoire_fond      = charger_bmp("victoire_fond.bmp");
    res->game_over_fond     = charger_bmp("game_over_fond.bmp");
    res->hud_fond           = charger_bmp("hud_fond.bmp");

    /* fond_menu est le seul obligatoire */
    if (!res->fond_menu) {
        allegro_message("ERREUR : fond.bmp introuvable dans %s", dossier);
        return false;
    }

    /* ── Boutons ─────────────────────────────────────────── */
    res->btn_jouer_idle     = charger_bmp("jouer_idle.bmp");
    res->btn_jouer_hover    = charger_bmp("jouer_hover.bmp");
    res->btn_regles_idle    = charger_bmp("regles_idle.bmp");
    res->btn_regles_hover   = charger_bmp("regles_hover.bmp");
    res->btn_options_idle   = charger_bmp("options_idle.bmp");
    res->btn_options_hover  = charger_bmp("options_hover.bmp");
    res->btn_credits_idle   = charger_bmp("credits_idle.bmp");
    res->btn_credits_hover  = charger_bmp("credits_hover.bmp");
    res->btn_nouvelle_idle  = charger_bmp("nouvelle_idle.bmp");
    res->btn_nouvelle_hover = charger_bmp("nouvelle_hover.bmp");
    res->btn_charger_idle   = charger_bmp("charger_idle.bmp");
    res->btn_charger_hover  = charger_bmp("charger_hover.bmp");
    res->btn_retour         = charger_bmp("retour.bmp");

    /* ── Fonds niveaux ───────────────────────────────────── */
    for (i = 0; i < NB_NIVEAUX; i++)
        res->fond_niveaux[i] = charger_bmp(noms_niv[i]);

    /* ── Joueur 1 ────────────────────────────────────────── */
    res->joueur_idle = charger_bmp("joueur_idle.bmp");
    for (i = 0; i < 6; i++) {
        char nom[64];
        snprintf(nom, sizeof(nom), "joueur_run%d.bmp", i);
        res->joueur_run[i] = charger_bmp(nom);
    }
    /* compatibilite joueur_marche */
    for (i = 0; i < 4; i++)
        res->joueur_marche[i] = res->joueur_run[i];

    /* ── Joueur 2 ────────────────────────────────────────── */
    res->joueur2_idle = charger_bmp("joueur2_idle.bmp");
    for (i = 0; i < 6; i++) {
        char nom[64];
        snprintf(nom, sizeof(nom), "joueur2_run%d.bmp", i);
        res->joueur2_run[i] = charger_bmp(nom);
    }

    /* ── Projectiles ─────────────────────────────────────── */
    res->kunai      = charger_bmp("kunai.bmp");
    res->eclair_spr = charger_bmp("eclair.bmp");

    for (i = 0; i < 4; i++) {
        char nom[64];
        snprintf(nom, sizeof(nom), "eclair_anim%d.bmp", i);
        res->eclair_anim[i] = charger_bmp(nom);
        snprintf(nom, sizeof(nom), "fireball%d.bmp", i);
        res->fireball[i] = charger_bmp(nom);
    }

    /* ── Bulles (optionnel — procédural si absent) ───────── */
    for (i = 1; i <= 3; i++) {
        char nom[64];
        snprintf(nom, sizeof(nom), "bulle%d.bmp", i);
        res->bulle[i] = charger_bmp(nom);
        snprintf(nom, sizeof(nom), "bulle%d_eclair.bmp", i);
        res->bulle_eclair[i] = charger_bmp(nom);
        snprintf(nom, sizeof(nom), "bulle%d_choc.bmp", i);
        res->bulle_choc[i] = charger_bmp(nom);
    }

    /* ── Bonus ───────────────────────────────────────────── */
    {
        const char *noms_bonus[NB_TYPES_BONUS] = {
            "bonus_x2.bmp", "bonus_x3.bmp", "bonus_laser.bmp",
            "bonus_temps.bmp", "bonus_invincible.bmp"
        };
        for (i = 0; i < NB_TYPES_BONUS; i++)
            res->bonus[i] = charger_bmp(noms_bonus[i]);
    }

    /* ── Boss ────────────────────────────────────────────── */
    res->boss_idle = charger_bmp("boss.bmp");
    res->boss_hit  = charger_bmp("boss_hit.bmp");
    for (i = 0; i < 4; i++) {
        char nom[64];
        snprintf(nom, sizeof(nom), "boss_run%d.bmp", i);
        res->boss_run[i] = charger_bmp(nom);
    }

    return true;
}

/* =========================================================
   LIBERATION
   ========================================================= */

void graphique_liberer_ressources(Ressources *res)
{
    int i;
#define F(b) do { if (b) { destroy_bitmap(b); b = NULL; } } while(0)

    F(res->fond_menu);        F(res->fond_jouer);
    F(res->panneau_geant);    F(res->choix_joueurs_fond);
    F(res->saisie_pseudo_fond); F(res->pause_fond);
    F(res->fin_niveau_fond);  F(res->victoire_fond);
    F(res->game_over_fond);   F(res->hud_fond);

    F(res->btn_jouer_idle);   F(res->btn_jouer_hover);
    F(res->btn_regles_idle);  F(res->btn_regles_hover);
    F(res->btn_options_idle); F(res->btn_options_hover);
    F(res->btn_credits_idle); F(res->btn_credits_hover);
    F(res->btn_nouvelle_idle);F(res->btn_nouvelle_hover);
    F(res->btn_charger_idle); F(res->btn_charger_hover);
    F(res->btn_retour);

    F(res->joueur_idle);      F(res->joueur2_idle);
    F(res->kunai);            F(res->eclair_spr);
    F(res->boss_idle);        F(res->boss_hit);

    for (i = 0; i < NB_NIVEAUX; i++) F(res->fond_niveaux[i]);
    for (i = 0; i < 6; i++) { F(res->joueur_run[i]); F(res->joueur2_run[i]); }
    for (i = 0; i < 4; i++) { F(res->fireball[i]); F(res->eclair_anim[i]); F(res->boss_run[i]); }
    for (i = 1; i <= 3; i++) { F(res->bulle[i]); F(res->bulle_eclair[i]); F(res->bulle_choc[i]); }
    for (i = 0; i < NB_TYPES_BONUS; i++) F(res->bonus[i]);

#undef F
}

/* =========================================================
   MENUS
   ========================================================= */

#define BTN_W 280
#define BTN_H  95

static void dessiner_bouton(BITMAP *buf, BITMAP *idle, BITMAP *hover,
                             int x, int y, int hov)
{
    BITMAP *img = (hov && hover) ? hover : idle;
    dessiner_image(buf, img, x, y, BTN_W, BTN_H);
}

void graphique_dessiner_menu_principal(BITMAP *buf,
                                       const Ressources *res,
                                       const int hover[4])
{
    int i;
    int ys[4] = {130, 250, 370, 490};
    BITMAP *idles[4]  = { res->btn_jouer_idle,   res->btn_regles_idle,
                           res->btn_options_idle,  res->btn_credits_idle };
    BITMAP *hovers[4] = { res->btn_jouer_hover,  res->btn_regles_hover,
                           res->btn_options_hover, res->btn_credits_hover };

    dessiner_fond(buf, res->fond_menu, WINDOW_W, WINDOW_H);
    for (i = 0; i < 4; i++)
        dessiner_bouton(buf, idles[i], hovers[i], 250, ys[i], hover[i]);
}

void graphique_dessiner_menu_jouer(BITMAP *buf,
                                   const Ressources *res,
                                   const int hover[2],
                                   int hover_retour)
{
    (void)hover_retour;
    dessiner_fond(buf, res->fond_jouer, WINDOW_W, WINDOW_H);
    dessiner_bouton(buf, res->btn_nouvelle_idle, res->btn_nouvelle_hover,
                    270, 270, hover[0]);
    dessiner_bouton(buf, res->btn_charger_idle,  res->btn_charger_hover,
                    270, 370, hover[1]);
    dessiner_image(buf, res->btn_retour, 90, 620, 120, 60);
}

void graphique_dessiner_ecran_texte(BITMAP *buf,
                                    const Ressources *res,
                                    EtatEcran etat,
                                    int hover_retour)
{
    int cx    = WINDOW_W / 2;
    int blanc = makecol(255, 255, 255);
    int or_   = makecol(255, 200, 0);
    int rouge = makecol(220, 60, 60);
    (void)hover_retour;

    dessiner_fond(buf, res->fond_menu, WINDOW_W, WINDOW_H);
    dessiner_image(buf, res->panneau_geant,
                   60, 60, WINDOW_W - 120, WINDOW_H - 120);

    switch (etat) {
        case ETAT_MENU_REGLES:
            graphique_textout_stylise(buf, "REGLES DU JEU", cx, 130, 5, or_, true);
            graphique_textout_stylise(buf, "- Eliminez toutes les bulles",      cx, 220, 2, blanc, true);
            graphique_textout_stylise(buf, "- Q/D pour courir",                 cx, 265, 2, blanc, true);
            graphique_textout_stylise(buf, "- ESPACE pour tirer",               cx, 310, 2, blanc, true);
            graphique_textout_stylise(buf, "- Les bulles se divisent",          cx, 355, 2, blanc, true);
            graphique_textout_stylise(buf, "- Niveau 3 : eclairs !",            cx, 400, 2, rouge, true);
            graphique_textout_stylise(buf, "- Niveau 5 : boss final !",         cx, 445, 2, rouge, true);
            graphique_textout_stylise(buf, "- Ramassez les bonus",              cx, 490, 2, blanc, true);
            graphique_textout_stylise(buf, "P = pause  |  ESC = quitter",       cx, 535, 2, blanc, true);
            break;
        case ETAT_MENU_OPTIONS:
            graphique_textout_stylise(buf, "OPTIONS",                           cx, 140, 5, or_,   true);
            graphique_textout_stylise(buf, "J1 : Q/D + ESPACE",                cx, 380, 2, blanc, true);
            graphique_textout_stylise(buf, "J2 : Fleches + Clic souris",       cx, 420, 2, blanc, true);
            break;
        case ETAT_MENU_CREDITS:
            graphique_textout_stylise(buf, "CREDITS",                           cx, 140, 5, or_,   true);
            graphique_textout_stylise(buf, "Projet ECE - Super Bulles 2025",   cx, 230, 3, blanc, true);
            graphique_textout_stylise(buf, "Guillaume MARMAGNE",                cx, 310, 3, blanc, true);
            graphique_textout_stylise(buf, "Aksel SAHALI",                      cx, 360, 3, blanc, true);
            graphique_textout_stylise(buf, "Baptiste ODIN",                     cx, 410, 3, blanc, true);
            graphique_textout_stylise(buf, "Sebastien NGUYEN",                  cx, 460, 3, blanc, true);
            break;
        default: break;
    }

    dessiner_image(buf, res->btn_retour, 90, 620, 120, 60);
}

void graphique_dessiner_saisie_pseudo(BITMAP *buf,
                                      const Ressources *res,
                                      const char *pseudo)
{
    char affichage[64];
    int cx = WINDOW_W / 2;
    BITMAP *fond = res->saisie_pseudo_fond
                   ? res->saisie_pseudo_fond
                   : res->fond_menu;

    dessiner_fond(buf, fond, WINDOW_W, WINDOW_H);

    graphique_textout_stylise(buf, "ENTREZ VOTRE PSEUDO",
                              cx, 240, 4, makecol(255, 200, 0), true);
    snprintf(affichage, sizeof(affichage), "%s_", pseudo);
    graphique_textout_stylise(buf, affichage,
                              cx, 350, 4, makecol(255, 255, 255), true);
    graphique_textout_stylise(buf, "Appuyez sur ENTREE pour valider",
                              cx, 460, 2, makecol(255, 255, 255), true);
}

void graphique_dessiner_choix_joueurs(BITMAP *buf,
                                      const Ressources *res,
                                      int mx, int my)
{
    int cx = WINDOW_W / 2;
    int bw = 300, bh = 70;
    int y1 = 280, y2 = 380, yr = 500;
    int hov1 = (mx>=cx-bw/2 && mx<=cx+bw/2 && my>=y1 && my<=y1+bh);
    int hov2 = (mx>=cx-bw/2 && mx<=cx+bw/2 && my>=y2 && my<=y2+bh);
    int hovr = (mx>=cx-bw/2 && mx<=cx+bw/2 && my>=yr && my<=yr+50);
    BITMAP *fond = res->choix_joueurs_fond
                   ? res->choix_joueurs_fond
                   : res->fond_menu;

    dessiner_fond(buf, fond, WINDOW_W, WINDOW_H);

    graphique_textout_stylise(buf, "NOMBRE DE JOUEURS",
                              cx, 150, 5, makecol(255, 200, 0), true);
    graphique_textout_stylise(buf, hov1 ? "> 1 JOUEUR <" : "1 JOUEUR",
                              cx, y1 + 20, 4, makecol(255, 255, 255), true);
    graphique_textout_stylise(buf, "Q/D + ESPACE",
                              cx, y1 + 50, 2, makecol(180, 180, 180), true);
    graphique_textout_stylise(buf, hov2 ? "> 2 JOUEURS <" : "2 JOUEURS",
                              cx, y2 + 20, 4, makecol(255, 255, 255), true);
    graphique_textout_stylise(buf, "J2 : Fleches + Clic",
                              cx, y2 + 50, 2, makecol(180, 180, 180), true);
    graphique_textout_stylise(buf, hovr ? "> RETOUR <" : "RETOUR",
                              cx, yr + 15, 3, makecol(255, 80, 80), true);
}

/* =========================================================
   JEU
   ========================================================= */

static void dessiner_joueur_sprite(BITMAP *buf, const Ressources *res,
                                    const Joueur *j, int frame, int j2)
{
    BITMAP *img = NULL;
    int dx, dy, anim, flip;

    if (!j || !j->vivant) return;
    if (j->invincible && (frame / 4) % 2 == 0) return;

    dx   = (int)j->x - PLAYER_W / 2;
    dy   = (int)j->y - PLAYER_H;
    anim = (frame / 6) % 6;
    flip = (j->vx < -0.1f);

    if (j2) {
        img = (j->vx > 0.1f || j->vx < -0.1f)
              ? res->joueur2_run[anim] : res->joueur2_idle;
        if (!img) img = res->joueur2_idle;
        if (!img) img = res->joueur_idle;   /* fallback J1 si J2 absent */
    } else {
        img = (j->vx > 0.1f || j->vx < -0.1f)
              ? res->joueur_run[anim] : res->joueur_idle;
        if (!img) img = res->joueur_idle;
    }

    if (!img) return;

    if (flip) {
        BITMAP *tmp = create_bitmap(PLAYER_W, PLAYER_H);
        if (!tmp) return;
        clear_to_color(tmp, makecol(255, 0, 255));
        masked_stretch_blit(img, tmp, 0, 0, img->w, img->h,
                            0, 0, PLAYER_W, PLAYER_H);
        draw_sprite_h_flip(buf, tmp, dx, dy);
        destroy_bitmap(tmp);
    } else {
        dessiner_image(buf, img, dx, dy, PLAYER_W, PLAYER_H);
    }
}

static void dessiner_projectiles(BITMAP *buf, const Ressources *res,
                                  const Projectile *p)
{
    for (; p; p = p->suivant) {
        if (!p->actif) continue;
        if (res->kunai)
            dessiner_image(buf, res->kunai,
                           (int)p->x, (int)p->y, PROJ_W, PROJ_H);
        else {
            int px2 = (int)p->x + PROJ_W / 2;
            line(buf, px2, (int)p->y, px2, (int)p->y + PROJ_H,
                 makecol(200, 200, 200));
        }
    }
}

static void dessiner_bulles(BITMAP *buf, const Ressources *res,
                             const Bulle *bulles)
{
    const Bulle *b;
    for (b = bulles; b; b = b->suivante) {
        int t, r;
        BITMAP *img;
        if (!b->active) continue;

        t = b->taille;
        if (t < 1) t = 1;
        if (t > 3) t = 3;
        r = BULLE_RAYON_BASE * t;

        /* choisir sprite */
        if (b->choc_timer > 0 && res->bulle_choc[t])
            img = res->bulle_choc[t];
        else if (b->lance_eclairs && res->bulle_eclair[t])
            img = res->bulle_eclair[t];
        else
            img = res->bulle[t];

        if (img) {
            dessiner_image(buf, img,
                           (int)b->x - r, (int)b->y - r, 2*r, 2*r);
        } else {
            /* fallback procédural verre */
            int cx=(int)b->x, cy=(int)b->y;
            circlefill(buf, cx+3, cy+3, r,   makecol(0,0,0));
            circlefill(buf, cx,   cy,   r,   makecol(180,220,255));
            if (r>2) circlefill(buf,cx,cy,r-2,makecol(210,235,255));
            if (r>4) circlefill(buf,cx,cy,r-4,makecol(235,248,255));
            circle(buf, cx, cy, r,   makecol(100,180,255));
            circle(buf, cx, cy, r-1, makecol(200,230,255));
            if (r/4>1) circlefill(buf,cx-r/3,cy-r/3,r/4,makecol(255,255,255));
            if (b->lance_eclairs) circle(buf,cx,cy,r+4,makecol(255,240,50));
            if (b->choc_timer>0) {
                int rc = r + (8 - b->choc_timer);
                circle(buf,cx,cy,rc,  makecol(255,255,255));
                circle(buf,cx,cy,rc+1,makecol(200,230,255));
            }
        }
    }
}

static void dessiner_eclairs(BITMAP *buf, const Ressources *res,
                              const Eclair *e, int frame)
{
    int anim = (frame / 4) % 4;
    for (; e; e = e->suivant) {
        if (!e->actif) continue;
        BITMAP *img = res->eclair_anim[anim]
                      ? res->eclair_anim[anim]
                      : res->eclair_spr;
        if (img)
            dessiner_image(buf, img, (int)e->x - 16, (int)e->y, 32, 80);
        else {
            line(buf,(int)e->x,(int)e->y,(int)e->x,(int)e->y+20,makecol(255,255,0));
            line(buf,(int)e->x-2,(int)e->y+5,(int)e->x+2,(int)e->y+15,makecol(255,220,100));
        }
    }
}

static void dessiner_bonus(BITMAP *buf, const Ressources *res,
                            const BonusItem *bon)
{
    for (; bon; bon = bon->suivant) {
        if (!bon->actif) continue;
        if (bon->type < 0 || bon->type >= NB_TYPES_BONUS) continue;
        if (res->bonus[bon->type])
            dessiner_image(buf, res->bonus[bon->type],
                           (int)bon->x - 18, (int)bon->y - 18, 36, 36);
        else {
            /* fallback procédural */
            int bcols[NB_TYPES_BONUS] = {
                makecol(80,180,255), makecol(80,255,120),
                makecol(255,200,30), makecol(180,255,180),
                makecol(255,80,200)
            };
            circlefill(buf,(int)bon->x,(int)bon->y,16,bcols[bon->type]);
            circle(buf,(int)bon->x,(int)bon->y,16,makecol(255,255,255));
        }
    }
}

static void dessiner_fireballs(BITMAP *buf, const Ressources *res,
                                const Fireball *pool, int nb)
{
    int i;
    for (i = 0; i < nb; i++) {
        int anim;
        if (!pool[i].actif) continue;
        anim = (pool[i].frame_anim / 4) % 4;
        if (res->fireball[anim])
            dessiner_image(buf, res->fireball[anim],
                           (int)pool[i].x - 16, (int)pool[i].y - 16, 32, 32);
        else {
            int cx=(int)pool[i].x, cy=(int)pool[i].y;
            circlefill(buf,cx,cy,12,makecol(255,140,0));
            circlefill(buf,cx,cy,7, makecol(255,220,50));
            circle(buf,cx,cy,13,   makecol(200,50,0));
        }
    }
}

static void dessiner_boss(BITMAP *buf, const Ressources *res,
                           const Boss *boss, int frame)
{
    BITMAP *img;
    int bx, by, anim;
    if (!boss || !boss->actif) return;

    bx   = (int)boss->x - 50;
    by   = (int)boss->y - 50;
    anim = (frame / 8) % 4;

    if (boss->hit_flash > 0 && res->boss_hit)
        img = res->boss_hit;
    else if (res->boss_run[anim])
        img = res->boss_run[anim];
    else
        img = res->boss_idle;

    if (img)
        dessiner_image(buf, img, bx, by, 100, 100);
    else {
        /* fallback procédural */
        circlefill(buf,(int)boss->x,(int)boss->y,50,makecol(180,30,30));
        circle(buf,(int)boss->x,(int)boss->y,50,makecol(100,0,0));
    }

    /* barre de vie */
    {
        int bw2 = 200;
        int bx2 = WINDOW_W/2 - bw2/2;
        int by2 = GAME_H - HUD_H - 20;
        int pv  = boss->pv * bw2 / boss->pv_max;
        rectfill(buf,bx2,by2,bx2+bw2,by2+14,makecol(80,0,0));
        rectfill(buf,bx2,by2,bx2+pv, by2+14,makecol(220,30,30));
        rect(buf,bx2,by2,bx2+bw2,by2+14,makecol(255,255,255));
        graphique_textout_stylise(buf,"BOSS",WINDOW_W/2,by2-18,2,
                                  makecol(255,80,80),true);
    }
}

void graphique_dessiner_hud(BITMAP *buf, const Ressources *res,
                             const Joueur *j, const Joueur *j2,
                             const EtatNiveau *niveau)
{
    int hy = WINDOW_H - HUD_H;
    char tmp[64];
    int col_temps;

    if (res->hud_fond)
        dessiner_image(buf, res->hud_fond, 0, hy, WINDOW_W, HUD_H);
    else {
        rectfill(buf, 0, hy, WINDOW_W, WINDOW_H, makecol(10,10,20));
        hline(buf, 0, hy, WINDOW_W-1, makecol(255,200,0));
    }

    /* J1 */
    snprintf(tmp, sizeof(tmp), "J1: %s", j->pseudo);
    graphique_textout_stylise(buf, tmp, 20, hy+8,  2, makecol(255,200,0),   false);
    snprintf(tmp, sizeof(tmp), "Score: %d", j->score);
    graphique_textout_stylise(buf, tmp, 20, hy+28, 2, makecol(255,255,255), false);

    /* Centre */
    col_temps = (niveau->temps_restant <= 15)
                ? makecol(255,80,80) : makecol(255,255,255);
    snprintf(tmp, sizeof(tmp), "TEMPS: %d", niveau->temps_restant);
    graphique_textout_stylise(buf, tmp, WINDOW_W/2, hy+8,  2, col_temps,        true);
    snprintf(tmp, sizeof(tmp), "NIV %d/%d", niveau->numero, NB_NIVEAUX);
    graphique_textout_stylise(buf, tmp, WINDOW_W/2, hy+30, 2, makecol(180,180,255), true);

    /* J2 */
    if (j2) {
        snprintf(tmp, sizeof(tmp), "J2: %s", j2->pseudo);
        graphique_textout_stylise(buf, tmp, WINDOW_W-220, hy+8,  2, makecol(80,220,255),  false);
        snprintf(tmp, sizeof(tmp), "Score: %d", j2->score);
        graphique_textout_stylise(buf, tmp, WINDOW_W-220, hy+28, 2, makecol(255,255,255), false);
        if (!j2->vivant)
            graphique_textout_stylise(buf, "MORT", WINDOW_W-220, hy+48, 2,
                                      makecol(255,60,60), false);
    }
}

void graphique_dessiner_jeu(BITMAP *buf, const Ressources *res,
                             const Joueur *j, const Joueur *j2,
                             const Bulle *bulles,
                             const Projectile *projs,
                             const Projectile *projs2,
                             const Eclair *eclairs,
                             const BonusItem *bonus,
                             const Boss *boss,
                             const Fireball *fireballs,
                             int nb_fireballs,
                             const EtatNiveau *niveau)
{
    int idx = niveau->numero - 1;
    static int frame = 0;
    frame++;
    if (idx < 0) idx = 0;
    if (idx >= NB_NIVEAUX) idx = NB_NIVEAUX - 1;

    dessiner_fond(buf, res->fond_niveaux[idx], WINDOW_W, GAME_H);
    dessiner_bonus(buf, res, bonus);
    dessiner_eclairs(buf, res, eclairs, frame);
    dessiner_fireballs(buf, res, fireballs, nb_fireballs);
    dessiner_bulles(buf, res, bulles);
    dessiner_projectiles(buf, res, projs);
    if (projs2) dessiner_projectiles(buf, res, projs2);
    dessiner_boss(buf, res, boss, frame);
    dessiner_joueur_sprite(buf, res, j,  frame, 0);
    if (j2) dessiner_joueur_sprite(buf, res, j2, frame, 1);
    graphique_dessiner_hud(buf, res, j, j2, niveau);
}

void graphique_dessiner_decompte(BITMAP *buf, const Ressources *res,
                                  int valeur)
{
    const char *msg = (valeur==0)?"GO!":(valeur==1)?"1":(valeur==2)?"2":"3";
    int col = (valeur==0) ? makecol(100,255,100) : makecol(255,255,80);
    (void)res;
    graphique_textout_stylise(buf, msg, WINDOW_W/2, GAME_H/2-60, 12, col, true);
}

/* =========================================================
   ECRANS DE FIN
   ========================================================= */

void graphique_dessiner_pause(BITMAP *buf, const Ressources *res,
                               int hover[2])
{
    int cx = WINDOW_W / 2;
    BITMAP *fond = res->pause_fond ? res->pause_fond : res->fond_menu;
    dessiner_fond(buf, fond, WINDOW_W, WINDOW_H);
    graphique_textout_stylise(buf, "PAUSE", cx, 200, 8, makecol(255,200,0), true);
    graphique_textout_stylise(buf, hover[0] ? "> Reprendre <"      : "Reprendre",
                              cx, 390, 3, makecol(255,255,255), true);
    graphique_textout_stylise(buf, hover[1] ? "> Menu Principal <" : "Menu Principal",
                              cx, 480, 3, makecol(255,255,255), true);
}

void graphique_dessiner_fin_niveau(BITMAP *buf, const Ressources *res,
                                   bool gagne, int score,
                                   const char *pseudo, int hover[4])
{
    int i, cx = WINDOW_W / 2;
    char tmp[64];
    const char *labels[4] = {"Quitter","Sauvegarder","Continuer","Menu"};
    BITMAP *fond = res->fin_niveau_fond ? res->fin_niveau_fond : res->fond_menu;

    dessiner_fond(buf, fond, WINDOW_W, WINDOW_H);
    graphique_textout_stylise(buf,
        gagne ? "NIVEAU TERMINE !" : "DEFAITE...",
        cx, 140, 6,
        gagne ? makecol(100,255,100) : makecol(255,80,80), true);

    snprintf(tmp, sizeof(tmp), "Ninja : %s", pseudo);
    graphique_textout_stylise(buf, tmp, cx, 240, 3, makecol(255,255,255), true);
    snprintf(tmp, sizeof(tmp), "Score : %d", score);
    graphique_textout_stylise(buf, tmp, cx, 290, 3, makecol(255,200,0),   true);

    for (i = 0; i < 4; i++) {
        char label[64];
        snprintf(label, sizeof(label), hover[i] ? "> %s <" : "%s", labels[i]);
        graphique_textout_stylise(buf, label, cx, 370 + i*70, 3,
                                  makecol(255,255,255), true);
    }
}

void graphique_dessiner_victoire(BITMAP *buf, const Ressources *res,
                                  int score, const char *pseudo)
{
    int cx = WINDOW_W / 2;
    char tmp[64];
    BITMAP *fond = res->victoire_fond ? res->victoire_fond : res->fond_menu;

    dessiner_fond(buf, fond, WINDOW_W, WINDOW_H);
    graphique_textout_stylise(buf, "VICTOIRE !!!", cx, 150, 8, makecol(255,220,0),   true);
    graphique_textout_stylise(buf, "Le demon est vaincu !", cx, 270, 3, makecol(255,255,255), true);
    snprintf(tmp, sizeof(tmp), "Ninja : %s", pseudo);
    graphique_textout_stylise(buf, tmp, cx, 350, 4, makecol(200,255,200), true);
    snprintf(tmp, sizeof(tmp), "Score final : %d", score);
    graphique_textout_stylise(buf, tmp, cx, 420, 4, makecol(255,200,0),   true);
    graphique_textout_stylise(buf, "Appuyez sur ENTREE", cx, 530, 3, makecol(255,255,255), true);
}

void graphique_dessiner_game_over(BITMAP *buf, const Ressources *res,
                                   int score, int hover[2])
{
    int cx = WINDOW_W / 2;
    char tmp[64];
    BITMAP *fond = res->game_over_fond ? res->game_over_fond : res->fond_menu;

    dessiner_fond(buf, fond, WINDOW_W, WINDOW_H);
    graphique_textout_stylise(buf, "GAME OVER", cx, 180, 8, makecol(200,0,0),     true);
    snprintf(tmp, sizeof(tmp), "Score : %d", score);
    graphique_textout_stylise(buf, tmp, cx, 330, 4, makecol(255,200,0),   true);
    graphique_textout_stylise(buf, hover[0] ? "> Rejouer <"       : "Rejouer",
                              cx, 460, 3, makecol(255,255,255), true);
    graphique_textout_stylise(buf, hover[1] ? "> Menu Principal <": "Menu Principal",
                              cx, 540, 3, makecol(255,255,255), true);
}