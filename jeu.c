/* =========================================================
 *  jeu.c  —  Boucle de jeu
 *  Appelé par menu()
 *  Appelle : logique.c (joueur/bulle/boss/niveau)
 *            graphique.c (affichage)
 * ========================================================= */

#include <allegro.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "types.h"
#include "jeu.h"
#include "logique.h"
#include "graphismes.h"
#include "ihm.h"

typedef enum {
    J_JEU = 0,
    J_PAUSE,
    J_FIN_NIVEAU,
    J_VICTOIRE,
    J_GAME_OVER,
    J_MENU,
    J_QUITTER
} EtatJeu;

typedef struct { int x, y, w, h; } Zone;
static int zone_hover(Zone z, int mx, int my)
{
    return (mx >= z.x && mx <= z.x+z.w &&
            my >= z.y && my <= z.y+z.h) ? 1 : 0;
}

void jeu(BITMAP *buffer, Ressources *res,
         const char *pseudo, int niveau_depart, int nb_joueurs)
{
    int i;

    /* ── Entités ─────────────────────────────────────────── */
    Joueur     joueur;     /* J1 — ZQSD + ESPACE            */
    Joueur     joueur2;    /* J2 — flèches + souris (2P)    */
    EtatNiveau niveau;
    Bulle     *bulles  = NULL;
    Projectile *projs  = NULL;
    Projectile *projs2 = NULL;   /* projectiles J2 */
    Eclair    *eclairs = NULL;
    BonusItem *bonus   = NULL;
    Boss       boss;
    bool       boss_actif;
    Fireball   fireballs[MAX_FIREBALLS];

    int hover_fin[4] = { 0,0,0,0 };
    int hover_go[2]  = { 0,0 };
    int countdown_val = 3;

    EtatJeu   etat = J_JEU;
    Cooldowns cd   = { 0, 0 };

    /* ── Init ────────────────────────────────────────────── */
    logique_init_joueur(&joueur, pseudo);
    joueur.id = 0;
    joueur.x  = WINDOW_W / 4.0f;   /* J1 côté gauche */

    logique_init_joueur(&joueur2, "Joueur 2");
    joueur2.id = 1;
    joueur2.x  = 3.0f * WINDOW_W / 4.0f;   /* J2 côté droit */

    logique_init_niveau(&niveau, niveau_depart);
    boss_actif = (niveau_depart == NB_NIVEAUX);
    logique_peupler_niveau(niveau.numero, &bulles,
                           boss_actif ? &boss : NULL, &joueur);
    fireball_init_pool(fireballs, MAX_FIREBALLS);
    niveau.countdown = 3 * FPS;
    countdown_val    = 3;

    /* ── Boucle jeu ──────────────────────────────────────── */
    while (etat != J_MENU && etat != J_QUITTER) {

        EntreesFrame entrees;
        ihm_capturer_entrees(&entrees, &cd);
        ihm_update_cooldowns(&cd);
        if (joueur.tir_cooldown  > 0) joueur.tir_cooldown--;
        if (joueur2.tir_cooldown > 0) joueur2.tir_cooldown--;

        /* ── Logique ───────────────────────────────────── */
        switch (etat) {

            /* ══ JEU ══════════════════════════════════════ */
            case J_JEU: {
                int  score_gagne;
                bool mort, bulles_ok, boss_ok, temps_ecoule;
                Bulle *b;

                if (niveau.countdown > 0) {
                    niveau.countdown--;
                    countdown_val = niveau.countdown / FPS;
                    break;
                }

                if (entrees.pause_key || entrees.echap) {
                    etat = J_PAUSE; cd.clic = 20; break;
                }

                /* ── J1 : ZQSD + ESPACE ────────────────── */
                {
                    bool g1 = key[KEY_Q];
                    bool d1 = key[KEY_D];
                    logique_deplacer_joueur(&joueur, g1, d1);
                    logique_update_joueur(&joueur);

                    if (key[KEY_SPACE]) {
                        Projectile *nouveau = NULL;
                        if (logique_tirer(&joueur, &nouveau,
                                          &joueur.tir_cooldown)) {
                            if (nouveau) {
                                nouveau->suivant = projs;
                                projs = nouveau;
                            }
                        }
                    }
                }

                /* ── J2 : flèches + clic souris ─────────── */
                if (nb_joueurs == 2 && joueur2.vivant) {
                    bool g2 = key[KEY_LEFT];
                    bool d2 = key[KEY_RIGHT];
                    logique_deplacer_joueur(&joueur2, g2, d2);
                    logique_update_joueur(&joueur2);

                    /* tir J2 : clic gauche souris */
                    if ((mouse_b & 1) && joueur2.tir_cooldown == 0) {
                        Projectile *nouveau = NULL;
                        if (logique_tirer(&joueur2, &nouveau,
                                          &joueur2.tir_cooldown)) {
                            if (nouveau) {
                                nouveau->suivant = projs2;
                                projs2 = nouveau;
                            }
                        }
                    }
                }

                /* bulles / éclairs / bonus / boss */
                logique_update_projectiles(&projs);
                if (nb_joueurs == 2)
                    logique_update_projectiles(&projs2);
                logique_update_bulles(&bulles, niveau.numero);

                b = bulles;
                while (b) {
                    if (b->active && b->lance_eclairs &&
                        b->eclair_timer == 0) {
                        Eclair *e   = logique_creer_eclair(b->x, b->y);
                        e->suivant  = eclairs;
                        eclairs     = e;
                        b->eclair_timer = 60 + rand() % 120;
                    }
                    b = b->suivante;
                }

                logique_update_eclairs(&eclairs);
                logique_update_bonus(&bonus);

                if (boss_actif && boss.actif) {
                    logique_update_boss(&boss, &bulles);
                    if (boss.spawn_timer == 0)
                        fireball_lancer(fireballs, MAX_FIREBALLS,
                                        boss.x, boss.y,
                                        joueur.x, joueur.y, 4.0f);
                }

                /* update + collision fireballs */
                fireball_update(fireballs, MAX_FIREBALLS);
                if (!joueur.invincible &&
                    fireball_touche_joueur(fireballs, MAX_FIREBALLS, &joueur))
                    joueur.vivant = false;
                if (nb_joueurs == 2 && !joueur2.invincible &&
                    fireball_touche_joueur(fireballs, MAX_FIREBALLS, &joueur2))
                    joueur2.vivant = false;

                /* collisions J1 */
                score_gagne = 0;
                mort = logique_tester_collisions(
                    &joueur, &projs, &bulles, &eclairs,
                    &bonus, boss_actif ? &boss : NULL,
                    &score_gagne, niveau.numero);
                joueur.score += score_gagne;

                /* collisions J2 */
                if (nb_joueurs == 2 && joueur2.vivant) {
                    int score2 = 0;
                    bool mort2 = logique_tester_collisions(
                        &joueur2, &projs2, &bulles, &eclairs,
                        &bonus, boss_actif ? &boss : NULL,
                        &score2, niveau.numero);
                    joueur2.score += score2;
                    joueur.score  += score2;   /* score partagé */
                    if (mort2) joueur2.vivant = false;
                }

                /* game over ou perte de vie */
                if (mort || (nb_joueurs == 1 && !joueur.vivant) ||
                    (nb_joueurs == 2 && !joueur.vivant && !joueur2.vivant)) {

                    /* perdre une vie */
                    joueur.vies--;
                    if (nb_joueurs == 2) joueur2.vies--;

                    if (joueur.vies <= 0) {
                        /* plus de vies → game over */
                        etat = J_GAME_OVER;
                        hover_go[0] = hover_go[1] = 0;
                        cd.clic = 30; break;
                    } else {
                        /* encore des vies → respawn sur le même niveau */
                        joueur.vivant     = true;
                        joueur.invincible = true;
                        joueur.invincible_timer = 3 * FPS; /* 3s invincible */
                        joueur.x = WINDOW_W / 2.0f;
                        joueur.y = GAME_H - PLAYER_H / 2.0f;
                        joueur.vx = 0;
                        if (nb_joueurs == 2) {
                            joueur2.vivant     = true;
                            joueur2.invincible = true;
                            joueur2.invincible_timer = 3 * FPS;
                            joueur2.x = 3.0f * WINDOW_W / 4.0f;
                            joueur2.y = GAME_H - PLAYER_H / 2.0f;
                            joueur2.vx = 0;
                        }
                        /* vider projectiles */
                        logique_liberer_projectiles(&projs);
                        logique_liberer_projectiles(&projs2);
                        fireball_init_pool(fireballs, MAX_FIREBALLS);
                        cd.clic = 20;
                    }
                }

                /* victoire niveau */
                bulles_ok = (logique_compter_bulles(bulles) == 0);
                boss_ok   = (!boss_actif || !boss.actif);
                if (bulles_ok && boss_ok) {
                    niveau.gagne = true;
                    etat         = J_FIN_NIVEAU;
                    for (i = 0; i < 4; i++) hover_fin[i] = 0;
                    cd.clic = 30; break;
                }

                /* chrono */
                temps_ecoule = logique_update_niveau(&niveau);
                if (temps_ecoule) {
                    niveau.gagne = false;
                    etat         = J_FIN_NIVEAU;
                    for (i = 0; i < 4; i++) hover_fin[i] = 0;
                    cd.clic = 30;
                }
                break;
            }

            /* ══ PAUSE ═════════════════════════════════════ */
            case J_PAUSE: {
                Zone zr = { WINDOW_W/2-130, 380, 260, 55 };
                Zone zm = { WINDOW_W/2-130, 470, 260, 55 };
                hover_go[0] = zone_hover(zr, entrees.souris_x, entrees.souris_y);
                hover_go[1] = zone_hover(zm, entrees.souris_x, entrees.souris_y);
                if (entrees.pause_key || entrees.echap) {
                    etat = J_JEU; cd.clic = 20; break;
                }
                if (entrees.clic_gauche) {
                    if (hover_go[0]) { etat = J_JEU;  cd.clic = 20; }
                    if (hover_go[1]) { etat = J_MENU; cd.clic = 20; }
                }
                break;
            }

            /* ══ FIN NIVEAU ════════════════════════════════ */
            case J_FIN_NIVEAU: {
                Zone zones[4] = {
                    { WINDOW_W/2-120, 370,       240, 50 },
                    { WINDOW_W/2-120, 370 + 70,  240, 50 },
                    { WINDOW_W/2-120, 370 + 140, 240, 50 },
                    { WINDOW_W/2-120, 370 + 210, 240, 50 }
                };
                for (i = 0; i < 4; i++)
                    hover_fin[i] = zone_hover(zones[i],
                        entrees.souris_x, entrees.souris_y);

                if (!entrees.clic_gauche) break;

                if (hover_fin[0]) {
                    etat = J_QUITTER;
                } else if (hover_fin[1]) {
                    logique_sauvegarder(joueur.pseudo, niveau.numero,
                                        joueur.score, "sauvegardes.txt");
                    allegro_message("Partie sauvegardee !");
                    cd.clic = 20;
                } else if (hover_fin[2]) {
                    cd.clic = 20;
                    if (niveau.gagne) {
                        int prochain = niveau.numero + 1;
                        if (prochain > NB_NIVEAUX) {
                            etat = J_VICTOIRE;
                        } else {
                            logique_init_niveau(&niveau, prochain);
                            logique_liberer_bulles(&bulles);
                            logique_liberer_projectiles(&projs);
                            logique_liberer_eclairs(&eclairs);
                            logique_liberer_bonus(&bonus);
                            boss_actif = (prochain == NB_NIVEAUX);
                            logique_peupler_niveau(prochain, &bulles,
                                boss_actif ? &boss : NULL, &joueur);
                            countdown_val    = 3;
                            niveau.countdown = 3 * FPS;
                            etat = J_JEU;
                        }
                    } else {
                        logique_init_niveau(&niveau, niveau.numero);
                        logique_liberer_bulles(&bulles);
                        logique_liberer_projectiles(&projs);
                        logique_liberer_eclairs(&eclairs);
                        logique_liberer_bonus(&bonus);
                        boss_actif = (niveau.numero == NB_NIVEAUX);
                        logique_peupler_niveau(niveau.numero, &bulles,
                            boss_actif ? &boss : NULL, &joueur);
                        countdown_val    = 3;
                        niveau.countdown = 3 * FPS;
                        joueur.vivant    = true;
                        etat = J_JEU;
                    }
                } else if (hover_fin[3]) {
                    etat = J_MENU; cd.clic = 20;
                }
                break;
            }

            /* ══ VICTOIRE ══════════════════════════════════ */
            case J_VICTOIRE:
                if (entrees.entree || entrees.clic_gauche) {
                    logique_sauvegarder(joueur.pseudo, NB_NIVEAUX,
                                        joueur.score, "sauvegardes.txt");
                    etat = J_MENU; cd.clic = 20;
                }
                break;

            /* ══ GAME OVER ═════════════════════════════════ */
            case J_GAME_OVER: {
                Zone zr = { WINDOW_W/2-130, 450, 260, 50 };
                Zone zm = { WINDOW_W/2-130, 530, 260, 50 };
                hover_go[0] = zone_hover(zr, entrees.souris_x, entrees.souris_y);
                hover_go[1] = zone_hover(zm, entrees.souris_x, entrees.souris_y);
                if (entrees.clic_gauche) {
                    if (hover_go[0]) {
                        logique_init_joueur(&joueur, pseudo);
                        logique_init_niveau(&niveau, 1);
                        logique_liberer_bulles(&bulles);
                        logique_liberer_projectiles(&projs);
                        logique_liberer_eclairs(&eclairs);
                        logique_liberer_bonus(&bonus);
                        boss_actif = false;
                        logique_peupler_niveau(1, &bulles, NULL, &joueur);
                        countdown_val    = 3;
                        niveau.countdown = 3 * FPS;
                        etat    = J_JEU;
                        cd.clic = 20;
                    }
                    if (hover_go[1]) {
                        etat = J_MENU; cd.clic = 20;
                    }
                }
                break;
            }

            default: break;
        }

        /* ── Rendu (graphique.c) ───────────────────────── */
        clear_to_color(buffer, makecol(0, 0, 0));
        switch (etat) {
            case J_JEU:
                graphique_dessiner_jeu(buffer, res,
                                       &joueur,
                                       nb_joueurs == 2 ? &joueur2 : NULL,
                                       bulles, projs, projs2,
                                       eclairs, bonus,
                                       boss_actif ? &boss : NULL,
                                       fireballs, MAX_FIREBALLS, &niveau);
                if (niveau.countdown > 0)
                    graphique_dessiner_decompte(buffer, res, countdown_val);
                break;
            case J_PAUSE:
                graphique_dessiner_jeu(buffer, res,
                                       &joueur,
                                       nb_joueurs == 2 ? &joueur2 : NULL,
                                       bulles, projs, projs2,
                                       eclairs, bonus,
                                       boss_actif ? &boss : NULL,
                                       fireballs, MAX_FIREBALLS, &niveau);
                graphique_dessiner_pause(buffer, res, hover_go);
                break;
            case J_FIN_NIVEAU:
                graphique_dessiner_fin_niveau(buffer, res,
                    niveau.gagne, joueur.score, joueur.pseudo, hover_fin);
                break;
            case J_VICTOIRE:
                graphique_dessiner_victoire(buffer, res,
                    joueur.score, joueur.pseudo);
                break;
            case J_GAME_OVER:
                graphique_dessiner_game_over(buffer, res,
                    joueur.score, hover_go);
                break;
            default: break;
        }

        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, WINDOW_W, WINDOW_H);
        rest(1000 / FPS);
    }

    /* ── Libération ─────────────────────────────────────── */
    logique_liberer_bulles(&bulles);
    logique_liberer_projectiles(&projs);
    logique_liberer_projectiles(&projs2);
    logique_liberer_eclairs(&eclairs);
    logique_liberer_bonus(&bonus);
}