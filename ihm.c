#include <allegro.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "ihm.h"

void ihm_capturer_entrees(EntreesFrame *e, Cooldowns *cd)
{
    e->souris_x      = mouse_x;
    e->souris_y      = mouse_y;
    e->clic_gauche   = ((mouse_b & 1) && cd->clic == 0);
    e->fleche_gauche = key[KEY_LEFT]  || key[KEY_A];
    e->fleche_droite = key[KEY_RIGHT] || key[KEY_D];
    e->tir           = key[KEY_SPACE] || key[KEY_UP];
    e->echap         = key[KEY_ESC];
    e->entree        = key[KEY_ENTER] || key[KEY_ENTER_PAD];
    e->pause_key     = key[KEY_P];
    e->texte_saisi   = '\0';
    e->backspace     = key[KEY_BACKSPACE];
    if (keypressed()) {
        int  k = readkey();
        char c = (char)(k & 0xFF);
        if (isprint((unsigned char)c))
            e->texte_saisi = c;
    }
}

void ihm_update_cooldowns(Cooldowns *cd)
{
    if (cd->clic > 0) cd->clic--;
    if (cd->tir  > 0) cd->tir--;
}

int ihm_check_hover(Bouton b, int mx, int my)
{
    return (mx >= b.x && mx <= b.x + b.w &&
            my >= b.y && my <= b.y + b.h) ? 1 : 0;
}

void ihm_init_bouton(Bouton *b, int x, int y, int w, int h,
                     BITMAP *idle, BITMAP *hover)
{
    b->x = x; b->y = y; b->w = w; b->h = h;
    b->img_idle   = idle;
    b->img_hover  = hover;
    b->is_hovered = 0;
}

bool ihm_bouton_clique(Bouton *b, const EntreesFrame *e)
{
    return (b->is_hovered && e->clic_gauche);
}

void ihm_update_hover_tab(Bouton *tab, int n, int mx, int my)
{
    int i;
    for (i = 0; i < n; i++)
        tab[i].is_hovered = ihm_check_hover(tab[i], mx, my);
}

bool ihm_saisir_pseudo(char *buf, int buf_len, const EntreesFrame *e)
{
    int len = (int)strlen(buf);
    if (e->backspace && len > 0) {
        buf[len - 1] = '\0';
    } else if (e->texte_saisi != '\0' && len < buf_len - 1) {
        buf[len]     = e->texte_saisi;
        buf[len + 1] = '\0';
    }
    return (e->entree && len > 0);
}