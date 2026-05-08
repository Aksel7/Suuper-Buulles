#include <allegro.h>
#include <stdio.h>
#include <stdbool.h>

//Dimensions de la fenêtre
#define WINDOW_W 1024
#define WINDOW_H 768

//Dimensions standard de nos boutons en bois
#define BUTTON_W 280
#define BUTTON_H 95

//Définition de nos "États" d'écran
#define ETAT_MENU_PRINCIPAL 0
#define ETAT_MENU_JOUER     1
#define ETAT_MENU_REGLES    2
#define ETAT_MENU_OPTIONS   3
#define ETAT_MENU_CREDITS   4

//Structure qui regroupe toutes les infos d'un bouton
typedef struct {
    int x;
    int y;
    int w;
    int h;
    BITMAP *img_idle;    //Image normale
    BITMAP *img_hover;   //Image quand la souris est dessus
    int is_hovered;      //1 si survolé, 0 sinon
} Bouton;

//Fonction pour écrire du texte stylisé (avec une ombre pour faire "Jeu Vidéo")

void textout_stylise(BITMAP *dest, const char *msg, int x, int y, int taille, int couleur, int centre) {
    int largeur_texte = text_length(font, msg);
    int hauteur_texte = 8;
    //On crée une petite image temporaire juste pour dessiner le texte
    BITMAP *image_temporaire_texte = create_bitmap(largeur_texte, hauteur_texte);

    int position_x_finale = x;
    if (centre == 1) {
        position_x_finale = x - (largeur_texte * taille) / 2;
    }

    clear_to_color(image_temporaire_texte, makecol(255, 0, 255));
    textout_ex(image_temporaire_texte, font, msg, 0, 0, makecol(0, 0, 0), -1);
    stretch_sprite(dest, image_temporaire_texte, position_x_finale + taille, y + taille, largeur_texte * taille, hauteur_texte * taille);

    //DESSINER LE TEXTE PRINCIPAL
    clear_to_color(image_temporaire_texte, makecol(255, 0, 255));
    textout_ex(image_temporaire_texte, font, msg, 0, 0, couleur, -1);
    stretch_sprite(dest, image_temporaire_texte, position_x_finale, y, largeur_texte * taille, hauteur_texte * taille);
    destroy_bitmap(image_temporaire_texte);
}

//Initialisation générale d'Allegro

void init_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();

    set_color_depth(desktop_color_depth());

    //Tentative d'ouverture de la fenêtre
    int ouverture_fenetre = set_gfx_mode(GFX_AUTODETECT_WINDOWED, WINDOW_W, WINDOW_H, 0, 0);
    if (ouverture_fenetre != 0) {
        allegro_message("Erreur d'initialisation : %s", allegro_error);
        exit(EXIT_FAILURE);
    }
}

//Fonction pour retirer le halo rose/magenta des images

void nettoyer_fond_magenta(BITMAP *bmp) {
    //On parcourt chaque pixel de l'image de haut en bas et de gauche à droite
    for (int y = 0; y < bmp->h; y++) {
        for (int x = 0; x < bmp->w; x++) {
            //On récupère la couleur du pixel actuel
            int couleur_pixel = getpixel(bmp, x, y);
            //On sépare le Rouge, le Vert et le Bleu
            int rouge = getr(couleur_pixel);
            int vert = getg(couleur_pixel);
            int bleu = getb(couleur_pixel);
            // On vérifie si c'est du rose
            if (rouge > 150 && bleu > 150 && vert < 120) {
                //Si c'est le cas, on remplace par le vrai magenta d'Allegro (255, 0, 255)
                //qui sera considéré comme transparent par draw_sprite et stretch_sprite
                putpixel(bmp, x, y, makecol(255, 0, 255));
            }
        }
    }
}

//Vérifie si la souris se trouve à l'intérieur d'un bouton

int check_hover(Bouton bouton_a_tester, int pos_souris_x, int pos_souris_y) {
    //On vérifie si la souris est entre le bord gauche et le bord droit du bouton
    bool est_dans_la_largeur = (pos_souris_x >= bouton_a_tester.x) && (pos_souris_x <= bouton_a_tester.x + bouton_a_tester.w);
    //On vérifie si la souris est entre le bord haut et le bord bas du bouton
    bool est_dans_la_hauteur = (pos_souris_y >= bouton_a_tester.y) && (pos_souris_y <= bouton_a_tester.y + bouton_a_tester.h);
    if (est_dans_la_largeur && est_dans_la_hauteur) {
        return 1; //La souris est sur le bouton
    } else {
        return 0; //La souris est en dehors
    }
}

//PROGRAMME PRINCIPAL

int main() {
    init_allegro();
    //CHARGEMENT DU MENU PRINCIPAL
    BITMAP *fond_principal = load_bitmap("fond.bmp", NULL);
    if (fond_principal == NULL) {
        allegro_message("Erreur: fond.bmp introuvable ! Vérifiez le dossier de votre projet.");
        exit(EXIT_FAILURE);
    }

    //Tableau de 4 boutons pour le menu d'accueil
    Bouton boutons_principal[4];

    boutons_principal[0].img_idle = load_bitmap("jouer_idle.bmp", NULL);
    boutons_principal[0].img_hover = load_bitmap("jouer_hover.bmp", NULL);

    boutons_principal[1].img_idle = load_bitmap("regles_idle.bmp", NULL);
    boutons_principal[1].img_hover = load_bitmap("regles_hover.bmp", NULL);

    boutons_principal[2].img_idle = load_bitmap("options_idle.bmp", NULL);
    boutons_principal[2].img_hover = load_bitmap("options_hover.bmp", NULL);

    boutons_principal[3].img_idle = load_bitmap("credits_idle.bmp", NULL);
    boutons_principal[3].img_hover = load_bitmap("credits_hover.bmp", NULL);

    //On configure les propriétés de base de chaque bouton
    for(int i = 0; i < 4; i++) {
        //on vérifie que les images ont bien été chargées
        if (boutons_principal[i].img_idle == NULL || boutons_principal[i].img_hover == NULL) {
            allegro_message("Erreur: Impossible de charger une image du menu principal.");
            exit(EXIT_FAILURE);
        }

        //Nettoyage des bords roses
        nettoyer_fond_magenta(boutons_principal[i].img_idle);
        nettoyer_fond_magenta(boutons_principal[i].img_hover);

        boutons_principal[i].w = BUTTON_W;
        boutons_principal[i].h = BUTTON_H;
        boutons_principal[i].is_hovered = 0;
    }

    // Positionnement des boutons sur les branches de l'arbre
    int ALIGNEMENT_X = 250;
    boutons_principal[0].x = ALIGNEMENT_X; boutons_principal[0].y = 130;
    boutons_principal[1].x = ALIGNEMENT_X; boutons_principal[1].y = 250;
    boutons_principal[2].x = ALIGNEMENT_X; boutons_principal[2].y = 370;
    boutons_principal[3].x = ALIGNEMENT_X; boutons_principal[3].y = 490;

    //CHARGEMENT DU SOUS-MENU "JOUER"

    BITMAP *fond_jouer = load_bitmap("fond_jouer.bmp", NULL);
    if (fond_jouer == NULL) {
        allegro_message("Erreur: fond_jouer.bmp introuvable !");
        exit(EXIT_FAILURE);
    }

    Bouton boutons_jouer[2];
    boutons_jouer[0].img_idle = load_bitmap("nouvelle_idle.bmp", NULL);
    boutons_jouer[0].img_hover = load_bitmap("nouvelle_hover.bmp", NULL);

    boutons_jouer[1].img_idle = load_bitmap("charger_idle.bmp", NULL);
    boutons_jouer[1].img_hover = load_bitmap("charger_hover.bmp", NULL);

    for(int i = 0; i < 2; i++) {
        if (boutons_jouer[i].img_idle == NULL || boutons_jouer[i].img_hover == NULL) {
            allegro_message("Erreur: Impossible de charger les images du sous-menu jouer");
            exit(EXIT_FAILURE);
        }
        nettoyer_fond_magenta(boutons_jouer[i].img_idle);
        nettoyer_fond_magenta(boutons_jouer[i].img_hover);

        boutons_jouer[i].w = BUTTON_W;
        boutons_jouer[i].h = BUTTON_H;
        boutons_jouer[i].is_hovered = 0;
    }

    int ALIGNEMENT_X_JOUER = 270;
    boutons_jouer[0].x = ALIGNEMENT_X_JOUER; boutons_jouer[0].y = 270;
    boutons_jouer[1].x = ALIGNEMENT_X_JOUER; boutons_jouer[1].y = 370;

    //CHARGEMENT DES AUTRES ELEMENTS (PANNEAUX ET RETOUR)

    //Le grand panneau en bois pour le texte
    BITMAP *panneau_geant = load_bitmap("panneau_geant.bmp", NULL);
    if (panneau_geant == NULL) {
        allegro_message("Erreur: panneau_geant.bmp introuvable !");
        exit(EXIT_FAILURE);
    }
    nettoyer_fond_magenta(panneau_geant);

    //Le bouton flèche pour faire "Retour"
    Bouton bouton_retour;
    bouton_retour.img_idle = load_bitmap("retour.bmp", NULL);
    if (bouton_retour.img_idle == NULL) {
        allegro_message("Erreur: retour.bmp introuvable !");
        exit(EXIT_FAILURE);
    }
    bouton_retour.img_hover = bouton_retour.img_idle; // On n'a pas d'image éclaircie pour la flèche
    nettoyer_fond_magenta(bouton_retour.img_idle);

    bouton_retour.w = 120;
    bouton_retour.h = 60;
    bouton_retour.x = 90; // Placé en bas à gauche de l'écran
    bouton_retour.y = 620;
    bouton_retour.is_hovered = 0;

    //PREPARATION DE LA BOUCLE DE JEU

    BITMAP *buffer = create_bitmap(WINDOW_W, WINDOW_H);
    int menu_running = 1;
    int click_cooldown = 0; // Temporisation pour éviter de détecter 50 clics par seconde
    int etat_courant = ETAT_MENU_PRINCIPAL;

    //BOUCLE PRINCIPALE

    while (menu_running == 1) {
        //GESTION DES ENTREES CLAVIER
        // Si on appuie sur Echap depuis le menu principal, on quitte le jeu
        if (key[KEY_ESC] && etat_courant == ETAT_MENU_PRINCIPAL && click_cooldown == 0) {
            menu_running = 0;
        }
        //GESTION DU BOUTON RETOUR
        // La flèche de retour n'est active que si on n'est PAS sur le menu principal
        if (etat_courant != ETAT_MENU_PRINCIPAL) {
            bouton_retour.is_hovered = check_hover(bouton_retour, mouse_x, mouse_y);
            //Si on clique sur le bouton retour
            if ((mouse_b & 1) && click_cooldown == 0 && bouton_retour.is_hovered == 1) {
                //printf("Retour au menu principal !\n");
                etat_courant = ETAT_MENU_PRINCIPAL;
                click_cooldown = 20; //On bloque le clic pendant quelques frames
            }
        }

        switch (etat_courant) {
            case ETAT_MENU_PRINCIPAL:
                //Mise à jour de l'état de survol
                for (int i = 0; i < 4; i++) {
                    boutons_principal[i].is_hovered = check_hover(boutons_principal[i], mouse_x, mouse_y);
                }

                //Détection du clic gauche
                if ((mouse_b & 1) && click_cooldown == 0) {
                    for (int i = 0; i < 4; i++) {
                        if (boutons_principal[i].is_hovered == 1) {
                            //On change d'état selon le bouton cliqué
                            if (i == 0)      etat_courant = ETAT_MENU_JOUER;
                            else if (i == 1) etat_courant = ETAT_MENU_REGLES;
                            else if (i == 2) etat_courant = ETAT_MENU_OPTIONS;
                            else if (i == 3) etat_courant = ETAT_MENU_CREDITS;

                            click_cooldown = 20;
                        }
                    }
                }
                break; //Fin du case ETAT_MENU_PRINCIPAL

            case ETAT_MENU_JOUER:
                for (int i = 0; i < 2; i++) {
                    boutons_jouer[i].is_hovered = check_hover(boutons_jouer[i], mouse_x, mouse_y);
                }

                if ((mouse_b & 1) && click_cooldown == 0) {
                    for (int i = 0; i < 2; i++) {
                        if (boutons_jouer[i].is_hovered == 1) {
                            if (i == 0) {
                                printf("Démarrage d'une NOUVELLE PARTIE !\n");
                            } else if (i == 1) {
                                printf("CHARGEMENT d'une ancienne sauvegarde !\n");
                            }
                            click_cooldown = 20;
                        }
                    }
                }
                break; //Fin du case ETAT_MENU_JOUER

            default:
                break;
        }

        if (click_cooldown > 0) {
            click_cooldown--;
        }

        switch (etat_courant) {

            case ETAT_MENU_PRINCIPAL:
                //Dessin du fond
                stretch_blit(fond_principal, buffer, 0, 0, fond_principal->w, fond_principal->h, 0, 0, WINDOW_W, WINDOW_H);

                //Dessin des boutons d'accueil
                for (int i = 0; i < 4; i++) {
                    BITMAP *img_a_dessiner;
                    if (boutons_principal[i].is_hovered == 1) {
                        img_a_dessiner = boutons_principal[i].img_hover;
                    } else {
                        img_a_dessiner = boutons_principal[i].img_idle;
                    }
                    stretch_sprite(buffer, img_a_dessiner, boutons_principal[i].x, boutons_principal[i].y, boutons_principal[i].w, boutons_principal[i].h);
                }
                break;

            case ETAT_MENU_JOUER:
                //Dessin du fond "Jouer"
                stretch_blit(fond_jouer, buffer, 0, 0, fond_jouer->w, fond_jouer->h, 0, 0, WINDOW_W, WINDOW_H);

                //Dessin des 2 boutons
                for (int i = 0; i < 2; i++) {
                    BITMAP *img_a_dessiner;
                    if (boutons_jouer[i].is_hovered == 1) {
                        img_a_dessiner = boutons_jouer[i].img_hover;
                    } else {
                        img_a_dessiner = boutons_jouer[i].img_idle;
                    }
                    stretch_sprite(buffer, img_a_dessiner, boutons_jouer[i].x, boutons_jouer[i].y, boutons_jouer[i].w, boutons_jouer[i].h);
                }

                //Dessin du bouton retour
                BITMAP *img_retour_j = bouton_retour.is_hovered ? bouton_retour.img_hover : bouton_retour.img_idle;
                stretch_sprite(buffer, img_retour_j, bouton_retour.x, bouton_retour.y, bouton_retour.w, bouton_retour.h);
                break;

            //Cas groupé pour les écrans avec le grand panneau (Règles, Options, Crédits)
            case ETAT_MENU_REGLES:
            case ETAT_MENU_OPTIONS:
            case ETAT_MENU_CREDITS:
                //Fond principal en arrière-plan
                stretch_blit(fond_principal, buffer, 0, 0, fond_principal->w, fond_principal->h, 0, 0, WINDOW_W, WINDOW_H);

                //Affichage du grand panneau par dessus
                int marge = 60;
                int rogne = 4;

                masked_stretch_blit(panneau_geant, buffer,
                                    rogne, rogne, panneau_geant->w - (2 * rogne), panneau_geant->h - (2 * rogne),
                                    marge, marge, WINDOW_W - (2 * marge), WINDOW_H - (2 * marge));

                // Affichage des textes selon l'état exact
                int col_texte = makecol(255, 255, 255); // Couleur Blanche
                int col_titre = makecol(255, 200, 0);   // Couleur Jaune/Dorée
                int centre_x = WINDOW_W / 2;

                if (etat_courant == ETAT_MENU_CREDITS) {
                    textout_stylise(buffer, "CREDITS", centre_x, 140, 5, col_titre, 1);
                    textout_stylise(buffer, "Projet ECE SuperPang", centre_x, 240, 3, col_texte, 1);
                    textout_stylise(buffer, "Cree par :", centre_x, 320, 2, col_texte, 1);
                    textout_stylise(buffer, "Guillaume MARMAGNE", centre_x, 380, 3, col_texte, 1);
                    textout_stylise(buffer, "Aksel SAHALI", centre_x, 430, 3, col_texte, 1);
                    textout_stylise(buffer, "Baptiste ODIN", centre_x, 480, 3, col_texte, 1);
                    textout_stylise(buffer, "Sebastien NGUYEN", centre_x, 530, 3, col_texte, 1);
                }
                else if (etat_courant == ETAT_MENU_REGLES) {
                    textout_stylise(buffer, "REGLES DU JEU", centre_x, 140, 5, col_titre, 1);
                    textout_stylise(buffer, "- Eliminez toutes les bulles du niveau", centre_x, 240, 2, col_texte, 1);
                    textout_stylise(buffer, "- Le joueur se deplace horizontalement", centre_x, 290, 2, col_texte, 1);
                    textout_stylise(buffer, "- Les bulles se divisent en les touchant", centre_x, 340, 2, col_texte, 1);
                    textout_stylise(buffer, "- Evitez bulles, eclairs et le Boss final", centre_x, 390, 2, col_texte, 1);
                    textout_stylise(buffer, "- Attention au chronometre !", centre_x, 440, 2, col_texte, 1);
                    textout_stylise(buffer, "- Battez le Boss au niveau 4 pour gagner", centre_x, 490, 2, col_texte, 1);
                }
                else if (etat_courant == ETAT_MENU_OPTIONS) {
                    textout_stylise(buffer, "OPTIONS", centre_x, 140, 5, col_titre, 1);
                    textout_stylise(buffer, "(En cours de developpement...)", centre_x, 300, 3, col_texte, 1);
                }

                //Affichage de la flèche de retour par dessus le panneau
                BITMAP *img_retour_p = bouton_retour.is_hovered ? bouton_retour.img_hover : bouton_retour.img_idle;
                stretch_sprite(buffer, img_retour_p, bouton_retour.x, bouton_retour.y, bouton_retour.w, bouton_retour.h);
                break;
        }

        //FINALISATION DE L'AFFICHAGE DE LA FRAME

        show_mouse(buffer);
        //On copie l'intégralité du buffer sur l'écran d'un seul coup
        blit(buffer, screen, 0, 0, 0, 0, WINDOW_W, WINDOW_H);
        rest(10);
    }

    //LIBERATION DE LA MEMOIRE
    destroy_bitmap(buffer);
    destroy_bitmap(fond_principal);
    destroy_bitmap(fond_jouer);
    destroy_bitmap(panneau_geant);
    destroy_bitmap(bouton_retour.img_idle);

    //Libération des tableaux de boutons
    for (int i = 0; i < 4; i++) {
        destroy_bitmap(boutons_principal[i].img_idle);
        destroy_bitmap(boutons_principal[i].img_hover);
    }
    for (int i = 0; i < 2; i++) {
        destroy_bitmap(boutons_jouer[i].img_idle);
        destroy_bitmap(boutons_jouer[i].img_hover);
    }

    return 0;
}
END_OF_MAIN()