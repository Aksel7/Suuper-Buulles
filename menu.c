/* =========================================================
 *  menu.c  —  Navigation menu (architecture Guillaume)
 *  Appelle jeu() quand le joueur lance une partie.
 * ========================================================= */

#include <allegro.h>
#include <string.h>
#include <stdbool.h>
#include "types.h"
#include "menu.h"
#include "jeu.h"
#include "ihm.h"
#include "graphismes.h"
#include "logique.h"

typedef enum {
    M_PRINCIPAL = 0,
    M_JOUER,
    M_REGLES,
    M_OPTIONS,
    M_CREDITS,
    M_PSEUDO,
    M_NB_JOUEURS,   /* choix 1 ou 2 joueurs */
    M_LANCER,
    M_QUITTER
} EtatMenu;

void menu(BITMAP *buffer, Ressources *res)
{
    int i;
    Bouton btn_p[4], btn_jouer[2], btn_retour;
    EtatMenu  etat       = M_PRINCIPAL;
    Cooldowns cd         = { 0, 0 };
    char      pseudo[32] = "";
    bool      charger_save = false;
    int       nb_joueurs   = 1;   /* 1 ou 2 */

    /* ── Init boutons ────────────────────────────────────── */
    {
        BITMAP *idles[4]  = { res->btn_jouer_idle,   res->btn_regles_idle,
                               res->btn_options_idle,  res->btn_credits_idle  };
        BITMAP *hovers[4] = { res->btn_jouer_hover,  res->btn_regles_hover,
                               res->btn_options_hover, res->btn_credits_hover };
        int ys[4] = { 130, 250, 370, 490 };
        for (i = 0; i < 4; i++)
            ihm_init_bouton(&btn_p[i], 250, ys[i], 280, 95,
                            idles[i], hovers[i]);
    }
    ihm_init_bouton(&btn_jouer[0], 270, 270, 280, 95,
                    res->btn_nouvelle_idle, res->btn_nouvelle_hover);
    ihm_init_bouton(&btn_jouer[1], 270, 370, 280, 95,
                    res->btn_charger_idle,  res->btn_charger_hover);
    ihm_init_bouton(&btn_retour, 90, 620, 120, 60,
                    res->btn_retour, res->btn_retour);

    /* ── Boucle menu ─────────────────────────────────────── */
    while (etat != M_QUITTER) {

        EntreesFrame entrees;
        ihm_capturer_entrees(&entrees, &cd);
        ihm_update_cooldowns(&cd);

        /* ── Logique ──────────────────────────────────────── */
        switch (etat) {

            case M_PRINCIPAL:
                ihm_update_hover_tab(btn_p, 4,
                                     entrees.souris_x, entrees.souris_y);
                btn_retour.is_hovered = 0;
                if (entrees.echap) { etat = M_QUITTER; break; }
                if (entrees.clic_gauche) {
                    for (i = 0; i < 4; i++) {
                        if (!btn_p[i].is_hovered) continue;
                        cd.clic = 20;
                        switch (i) {
                            case 0: etat = M_JOUER;   break;
                            case 1: etat = M_REGLES;  break;
                            case 2: etat = M_OPTIONS; break;
                            case 3: etat = M_CREDITS; break;
                        }
                    }
                }
                break;

            case M_JOUER:
                ihm_update_hover_tab(btn_jouer, 2,
                                     entrees.souris_x, entrees.souris_y);
                btn_retour.is_hovered = ihm_check_hover(btn_retour,
                    entrees.souris_x, entrees.souris_y);
                if (entrees.echap ||
                    ihm_bouton_clique(&btn_retour, &entrees)) {
                    etat = M_PRINCIPAL; cd.clic = 20; break;
                }
                if (entrees.clic_gauche) {
                    if (btn_jouer[0].is_hovered) {
                        charger_save = false;
                        pseudo[0]    = '\0';
                        etat         = M_PSEUDO;
                        cd.clic      = 20;
                    }
                    if (btn_jouer[1].is_hovered) {
                        charger_save = true;
                        pseudo[0]    = '\0';
                        etat         = M_PSEUDO;
                        cd.clic      = 20;
                    }
                }
                break;

            case M_REGLES:
            case M_OPTIONS:
            case M_CREDITS:
                btn_retour.is_hovered = ihm_check_hover(btn_retour,
                    entrees.souris_x, entrees.souris_y);
                if (entrees.echap ||
                    ihm_bouton_clique(&btn_retour, &entrees)) {
                    etat = M_PRINCIPAL; cd.clic = 20;
                }
                break;

            case M_PSEUDO:
                if (ihm_saisir_pseudo(pseudo, 32, &entrees)) {
                    if (!charger_save) {
                        /* Nouvelle partie : vérifier si le nom existe déjà */
                        if (logique_save_existe(pseudo)) {
                            allegro_message(
                                "Le nom '%s' existe deja !\n"
                                "Choisissez un autre nom ou\n"
                                "utilisez 'Charger' pour reprendre.", pseudo);
                            pseudo[0] = '\0';  /* vider et recommencer */
                        } else {
                            etat = M_NB_JOUEURS;
                            cd.clic = 20;
                        }
                    } else {
                        /* Charger : vérifier que la save existe */
                        if (!logique_save_existe(pseudo)) {
                            allegro_message(
                                "Aucune sauvegarde pour '%s'.\n"
                                "Verifiez le nom ou creez une nouvelle partie.", pseudo);
                            pseudo[0] = '\0';
                        } else {
                            etat = M_LANCER;
                            cd.clic = 20;
                        }
                    }
                }
                if (entrees.echap) {
                    etat      = M_PRINCIPAL;
                    pseudo[0] = '\0';
                }
                break;

            case M_NB_JOUEURS: {
                int cx = WINDOW_W / 2;
                int y1 = 280, y2 = 380, yr = 500;
                int bw = 300, bh = 70;
                int mx = entrees.souris_x, my = entrees.souris_y;

                int hov1 = (mx>=cx-bw/2 && mx<=cx+bw/2 && my>=y1 && my<=y1+bh);
                int hov2 = (mx>=cx-bw/2 && mx<=cx+bw/2 && my>=y2 && my<=y2+bh);
                int hovr = (mx>=cx-bw/2 && mx<=cx+bw/2 && my>=yr && my<=yr+50);

                if (entrees.echap) { etat = M_PSEUDO; cd.clic = 20; break; }

                if (entrees.clic_gauche) {
                    if (hov1) { nb_joueurs = 1; etat = M_LANCER; cd.clic = 20; }
                    if (hov2) { nb_joueurs = 2; etat = M_LANCER; cd.clic = 20; }
                    if (hovr) { etat = M_PSEUDO; cd.clic = 20; }
                }
                break;
            }

            case M_LANCER: {
                int niveau_depart = 1;
                int vies_depart   = 3;
                int score_depart  = 0;
                if (charger_save) {
                    int niv_save = 1, score_save = 0, nb_j_save = 1, vies_save = 3;
                    if (logique_charger(pseudo, &niv_save,
                                        &score_save, &nb_j_save, &vies_save, NULL)) {
                        niveau_depart = niv_save + 1;
                        if (niveau_depart > NB_NIVEAUX)
                            niveau_depart = NB_NIVEAUX;
                        nb_joueurs   = nb_j_save;
                        vies_depart  = vies_save;
                        score_depart = score_save;
                    }
                }
                jeu(buffer, res, pseudo, niveau_depart, nb_joueurs, vies_depart, score_depart);
                etat    = M_PRINCIPAL;
                cd.clic = 30;
                break;
            }
        }

        /* ── Rendu menu (graphique.c) ────────────────────── */
        clear_to_color(buffer, makecol(0, 0, 0));
        {
            int hp[4], hj[2];
            for (i = 0; i < 4; i++) hp[i] = btn_p[i].is_hovered;
            for (i = 0; i < 2; i++) hj[i] = btn_jouer[i].is_hovered;

            switch (etat) {
                case M_PRINCIPAL:
                    graphique_dessiner_menu_principal(buffer, res, hp);
                    break;
                case M_JOUER:
                    graphique_dessiner_menu_jouer(buffer, res, hj,
                                                  btn_retour.is_hovered);
                    break;
                case M_REGLES:
                    graphique_dessiner_ecran_texte(buffer, res,
                        ETAT_MENU_REGLES, btn_retour.is_hovered);
                    break;
                case M_OPTIONS:
                    graphique_dessiner_ecran_texte(buffer, res,
                        ETAT_MENU_OPTIONS, btn_retour.is_hovered);
                    break;
                case M_CREDITS:
                    graphique_dessiner_ecran_texte(buffer, res,
                        ETAT_MENU_CREDITS, btn_retour.is_hovered);
                    break;
                case M_PSEUDO:
                    graphique_dessiner_saisie_pseudo(buffer, res, pseudo);
                    break;
                case M_NB_JOUEURS:
                    graphique_dessiner_choix_joueurs(buffer, res,
                        entrees.souris_x, entrees.souris_y);
                    break;
                default: break;
            }
        }

        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, WINDOW_W, WINDOW_H);
        rest(1000 / FPS);
    }
}