#ifndef LOGIQUE_H
#define LOGIQUE_H

#include <stdbool.h>
#include "types.h"

/* Joueur */
void logique_init_joueur    (Joueur *j, const char *pseudo);
void logique_deplacer_joueur(Joueur *j, bool gauche, bool droite);
void logique_update_joueur  (Joueur *j);

/* Projectiles */
Projectile *logique_creer_projectile  (float x, float y, TypeArme arme);
bool        logique_tirer             (Joueur *j, Projectile **liste, int *cd);
void        logique_update_projectiles(Projectile **liste);
void        logique_liberer_projectiles(Projectile **liste);

/* Bulles */
Bulle *logique_creer_bulle  (float x, float y, int taille, bool eclairs);
void   logique_update_bulles(Bulle **liste, int niveau);
void   logique_liberer_bulles(Bulle **liste);
int    logique_compter_bulles(Bulle *liste);
int    logique_diviser_bulle (Bulle **liste, Bulle *cible,
                               BonusItem **bonus, int niveau);

/* Éclairs */
Eclair *logique_creer_eclair   (float x, float y);
void    logique_update_eclairs (Eclair **liste);
void    logique_liberer_eclairs(Eclair **liste);

/* Bonus */
BonusItem *logique_creer_bonus    (float x, float y, TypeBonus type);
void       logique_update_bonus   (BonusItem **liste);
void       logique_liberer_bonus  (BonusItem **liste);
void       logique_appliquer_bonus(Joueur *j, TypeBonus type,
                                   EtatNiveau *niveau);

/* Boss */
void logique_init_boss  (Boss *b);
void logique_update_boss(Boss *b, Bulle **bulles);
bool logique_hit_boss   (Boss *b);

/* Collisions */
bool logique_collision_cercle_rect(float cx, float cy, float r,
                                   float rx, float ry, float rw, float rh);
bool logique_collision_rect_rect  (float ax, float ay, float aw, float ah,
                                   float bx, float by, float bw, float bh);
bool logique_tester_collisions    (Joueur *j, Projectile **projs,
                                   Bulle **bulles, Eclair **eclairs,
                                   BonusItem **bonus, Boss *boss,
                                   int *score, int niveau,
                                   EtatNiveau *etat_niveau);

/* Niveau */
void logique_init_niveau   (EtatNiveau *n, int numero);
bool logique_update_niveau (EtatNiveau *n);
void logique_peupler_niveau(int numero, Bulle **bulles,
                             Boss *boss, Joueur *j);

/* Fireball (boss) */
void fireball_init_pool  (Fireball *pool, int nb);
void fireball_lancer     (Fireball *pool, int nb,
                          float bx, float by,
                          float cible_x, float cible_y,
                          float vitesse);
void fireball_update     (Fireball *pool, int nb);
int  fireball_touche_joueur(Fireball *pool, int nb, const Joueur *j);

/* Sauvegarde — un fichier par joueur : <pseudo>.txt */
int  logique_save_existe  (const char *pseudo);
void logique_sauvegarder  (const char *pseudo, int niveau, int score,
                            int nb_joueurs, int vies,
                            const char *fichier_ignore);
bool logique_charger       (const char *pseudo, int *niveau, int *score,
                            int *nb_joueurs, int *vies,
                            const char *fichier_ignore);

#endif /* LOGIQUE_H */