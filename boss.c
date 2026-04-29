#include "boss.h"

#define SCREEN_W 800

void init_boss(Boss *boss, int pv_depart) {
    boss->x = 350;
    boss->y = 60;

    boss->largeur = 100;
    boss->hauteur = 80;

    boss->vx = 2;

    boss->pv = pv_depart;
    boss->pv_max = pv_depart;
    boss->actif = 1;

    boss->etat = BOSS_PATTERN_DEPLACEMENT;
    boss->timer_pattern = 0;
}

void update_boss(Boss *boss) {
    if (!boss->actif) return;

    boss->timer_pattern++;

    if (boss->pv <= boss->pv_max / 3) {
        boss->etat = BOSS_PATTERN_ENRAGE;
        boss->vx = 5;
    }
    else if (boss->timer_pattern > 180) {
        boss->etat = BOSS_PATTERN_ATTAQUE;
    }
    else {
        boss->etat = BOSS_PATTERN_DEPLACEMENT;
    }

    boss->x += boss->vx;

    if (boss->x < 0) {
        boss->x = 0;
        boss->vx = -boss->vx;
    }

    if (boss->x + boss->largeur > SCREEN_W) {
        boss->x = SCREEN_W - boss->largeur;
        boss->vx = -boss->vx;
    }

    if (boss->timer_pattern > 240) {
        boss->timer_pattern = 0;
    }
}

void toucher_boss(Boss *boss, int degats) {
    if (!boss->actif) return;

    boss->pv -= degats;

    if (boss->pv <= 0) {
        boss->pv = 0;
        boss->actif = 0;
    }
}

int boss_est_vaincu(Boss *boss) {
    return boss->pv <= 0 || boss->actif == 0;
}