#ifndef NIVEAUX_H
#define NIVEAUX_H

#define NB_NIVEAUX 4
#define MAX_BULLES_INIT 10
#define MAX_SLIMES_INIT 5

typedef enum {
    OBJ_ELIMINER_BULLES,
    OBJ_ELIMINER_BULLES_ET_BOSS
} ObjectifNiveau;

typedef struct {
    float x, y;
    float vx, vy;
    float rayon;
    int division;
    int lance_eclairs;
} BulleInit;

typedef struct {
    float x, y;
    float vx, vy;
    float rayon;
    int pv;
} SlimeInit;

typedef struct {
    int active;
    int delai_min_spawn;
    int delai_max_spawn;
    float vitesse_y;
    float rayon;
} BouleFeuConfig;

typedef struct {
    int numero;
    char nom[50];
    char decor[100];

    int temps_limite;
    ObjectifNiveau objectif;

    int nb_bulles_initiales;
    BulleInit bulles[MAX_BULLES_INIT];

    int nb_slimes_initiales;
    SlimeInit slimes[MAX_SLIMES_INIT];

    BouleFeuConfig boule_feu;

    int boss_present;
    int pv_boss;

} NiveauConfig;

void charger_niveau(NiveauConfig *niveau, int numero);

#endif