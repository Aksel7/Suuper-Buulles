#ifndef IHM_H
#define IHM_H

#include <stdbool.h>
#include "types.h"

void ihm_capturer_entrees  (EntreesFrame *e, Cooldowns *cd);
void ihm_update_cooldowns  (Cooldowns *cd);
int  ihm_check_hover       (Bouton b, int mx, int my);
void ihm_init_bouton       (Bouton *b, int x, int y, int w, int h,
                             BITMAP *idle, BITMAP *hover);
bool ihm_bouton_clique     (Bouton *b, const EntreesFrame *e);
void ihm_update_hover_tab  (Bouton *tab, int n, int mx, int my);
bool ihm_saisir_pseudo     (char *buf, int buf_len, const EntreesFrame *e);

#endif /* IHM_H */