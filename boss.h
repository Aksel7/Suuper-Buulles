#ifndef SUPERPONG_BOSS_H
#define SUPERPONG_BOSS_H

typedef enum {
    BOSS_PATTERN_DEPLACEMENT,
    BOSS_PATTERN_ATTAQUE,
    BOSS_PATTERN_ENRAGE
} EtatBoss;

typedef struct {
    float x, y;
    float vx;
    int largeur, hauteur;

    int pv;
    int pv_max;
    int actif;

    EtatBoss etat;
    int timer_pattern;
} Boss;

void init_boss(Boss *boss, int pv_depart);
void update_boss(Boss *boss);
void toucher_boss(Boss *boss, int degats);
int boss_est_vaincu(Boss *boss);

#endif //SUPERPONG_BOSS_H