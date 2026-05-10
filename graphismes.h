#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include <allegro.h>
#include <stdbool.h>
#include "types.h"

/* =========================================================
   GRAPHIQUE.H — Couche d'affichage Allegro 4
   Appelée par jeu.c et menu.c — jamais par logique.c
   ========================================================= */

/* Init & ressources */
void graphique_init_allegro        (void);
bool graphique_charger_ressources  (Ressources *res, const char *dossier);
void graphique_liberer_ressources  (Ressources *res);

/* Utilitaires */
void graphique_nettoyer_magenta    (BITMAP *bmp);
void graphique_textout_stylise     (BITMAP *dest, const char *msg,
                                    int x, int y, int taille,
                                    int couleur, bool centre);

/* ── Menus ─────────────────────────────────────────────── */
void graphique_dessiner_menu_principal (BITMAP *buf, const Ressources *res,
                                        const int hover[4]);
void graphique_dessiner_menu_jouer     (BITMAP *buf, const Ressources *res,
                                        const int hover[2], int hover_retour);
void graphique_dessiner_ecran_texte    (BITMAP *buf, const Ressources *res,
                                        EtatEcran etat, int hover_retour);
void graphique_dessiner_saisie_pseudo  (BITMAP *buf, const Ressources *res,
                                        const char *pseudo);
void graphique_dessiner_choix_joueurs  (BITMAP *buf, const Ressources *res,
                                        int mx, int my);

/* ── Jeu ───────────────────────────────────────────────── */
void graphique_dessiner_jeu      (BITMAP *buf, const Ressources *res,
                                   const Joueur *j,
                                   const Joueur *j2,          /* NULL si 1 joueur */
                                   const Bulle *bulles,
                                   const Projectile *projs,
                                   const Projectile *projs2,  /* NULL si 1 joueur */
                                   const Eclair *eclairs,
                                   const BonusItem *bonus,
                                   const Boss *boss,
                                   const Fireball *fireballs,
                                   int nb_fireballs,
                                   const EtatNiveau *niveau);
void graphique_dessiner_hud      (BITMAP *buf, const Ressources *res,
                                   const Joueur *j, const Joueur *j2,
                                   const EtatNiveau *niveau);
void graphique_dessiner_decompte (BITMAP *buf, const Ressources *res,
                                   int valeur);

/* ── Écrans de fin ─────────────────────────────────────── */
void graphique_dessiner_pause      (BITMAP *buf, const Ressources *res,
                                    int hover[2]);
void graphique_dessiner_fin_niveau (BITMAP *buf, const Ressources *res,
                                    bool gagne, int score,
                                    const char *pseudo, int hover[4]);
void graphique_dessiner_victoire   (BITMAP *buf, const Ressources *res,
                                    int score, const char *pseudo);
void graphique_dessiner_game_over  (BITMAP *buf, const Ressources *res,
                                    int score, int hover[2]);

#endif /* GRAPHIQUE_H */