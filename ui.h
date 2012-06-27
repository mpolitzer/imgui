#ifndef UI_H
#define UI_H

#include <GL/gl.h>
#include <allegro5/allegro.h>

void ui_update(ALLEGRO_EVENT *ev);
void ui_begin(int w, int h);
void ui_end(void);
int ui_button(int id, int x, int y, int w, int h);
int ui_drag(int id, int x, int y, int w, int h, int *dx, int *dy);
int ui_drop(int id, int x, int y, int w, int h, int dx, int dy);
int ui_vslider(int id, int x, int y, int max, int *value);

#endif /* UI_H */
