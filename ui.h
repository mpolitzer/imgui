#ifndef UI_H
#define UI_H

#include <GL/gl.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

struct rect {
	short x, y;
	unsigned short w, h;
};

struct rect mkrect(short x, short y, unsigned short w, unsigned short h);

void ui_begin(int w, int h);
int ui_end(void);
int ui_update(ALLEGRO_EVENT *ev);
#if 0
void ui_setstyle(ALLEGRO_FONT *f,
		ALLEGRO_COLOR fg,
		ALLEGRO_COLOR bg,
		ALLEGRO_COLOR over);

/* widgets */
int ui_button(int id, struct rect r);
int ui_lbutton(int id, struct rect r, const char *label);
int ui_drag(int id, struct rect r, int *x, int *y);
int ui_drop(int id, struct rect r, int x, int y);
int ui_vslider(int id, struct rect r, int max, int *value);
int ui_hslider(int id, struct rect r, int max, int *value);
void ui_label(int id, struct rect r, const char *label);
int ui_textbox(int id, struct rect r, const char *label);
#endif
/* widgets */
int ui_button(int id, int x, int y, int w, int h);
int ui_drag(int id, int x, int y, int w, int h, int *dx, int *dy);
int ui_drop(int id, int x, int y, int w, int h, int dx, int dy);
int ui_vslider(int id, int x, int y, int max, int *value);

#endif /* UI_H */
