#ifndef UI_H
#define UI_H

#include <GL/gl.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

typedef struct {
	short x, y;
	unsigned short w, h;
} rect_t;

rect_t ui_mkrect(short x, short y, unsigned short w, unsigned short h);

void ui_setstyle_default(void);
void ui_setstyle(const char *font12,	/* bitmap monospace font. */
		ALLEGRO_COLOR active,
		ALLEGRO_COLOR hot,
		ALLEGRO_COLOR away,
		ALLEGRO_COLOR shadow,
		ALLEGRO_COLOR kbdfocus);

void ui_begin(int w, int h);
int ui_end(void);
int ui_update(ALLEGRO_EVENT *ev);

/* widgets */
int ui_button(int id, rect_t r);
int ui_buttonl(int id, rect_t r, const char *label);
int ui_drag(int id, rect_t r, int *dx, int *dy);
int ui_drop(int id, rect_t r, int dx, int dy);
void ui_label(int id, rect_t r, const char *s);

#endif /* UI_H */
