#include "ui.h"

struct uistate {
	int mousex;
	int mousey;
	int mousedown;

	int hotitem;
	int activeitem;

	int kbditem;
	int keyentered;
	int keymod;
	int keychar;

	int lastwidget;

	int dragid; /* store the ID that issued a ui_drag */
} uistate;

struct uistyle {
	ALLEGRO_BITMAP *stylesheet;
	ALLEGRO_BITMAP *font12;
	float fontw, fonth;
	ALLEGRO_COLOR hot, active, away, shadow, kbdfocus;
} uistyle;

static int rect_hitxy(rect_t r, int x, int y);
static int rect_hit(rect_t r);
static void rect_draw(rect_t r, ALLEGRO_COLOR c);
static void drawchar(int x, int y, unsigned char c);

void ui_finish(void)
{
	al_destroy_bitmap(uistyle.stylesheet);
	al_destroy_bitmap(uistyle.font12);
}

int ui_update(ALLEGRO_EVENT *ev)
{
	switch (ev->type) {
	case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		uistate.mousedown = 0;
		break;
	case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		uistate.mousedown = 1;
		break;
	case ALLEGRO_EVENT_MOUSE_AXES:
		uistate.mousex = ev->mouse.x;
		uistate.mousey = ev->mouse.y;
		break;
	case ALLEGRO_EVENT_KEY_DOWN:
		uistate.keyentered = ev->keyboard.keycode;
		uistate.keymod = ev->keyboard.modifiers;
		break;
	case ALLEGRO_EVENT_KEY_UP:
		uistate.keyentered = 0;
		uistate.keymod = 0;
		break;
	case ALLEGRO_EVENT_KEY_CHAR:
		uistate.keychar = ev->keyboard.unichar;
		break;
	default: return 0;
	}
	return 1;
}

void ui_begin(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	uistate.hotitem = 0;
}

int ui_end(void)
{
	int ui_state = uistate.activeitem;
	if (uistate.mousedown == 0) uistate.activeitem = 0;
	else if (uistate.activeitem == 0) uistate.activeitem = -1;
	if (uistate.keyentered == ALLEGRO_KEY_TAB) uistate.kbditem = 0;
	uistate.keyentered = 0;
	uistate.keychar = 0;
	return (ui_state > 0) ? 1 : 0;
}

void ui_setstyle(const char *font12,
		ALLEGRO_COLOR active,
		ALLEGRO_COLOR hot,
		ALLEGRO_COLOR away,
		ALLEGRO_COLOR shadow,
		ALLEGRO_COLOR kbdfocus)
{
	uistyle.font12 = al_load_bitmap(font12);
	uistyle.hot = hot;
	uistyle.active = active;
	uistyle.away = away;
	uistyle.kbdfocus = al_map_rgba(150, 0, 0, 127);

	uistyle.fontw = al_get_bitmap_width(uistyle.font12) / (95.0);
	uistyle.fonth = al_get_bitmap_height(uistyle.font12);
}

void ui_setstyle_default(void)
{
	ui_setstyle("resources/vera-mono-12.png",
			al_map_rgba(255, 255, 255, 200),
			al_map_rgba(230, 230, 230, 200),
			al_map_rgba(200, 200, 200, 200),
			al_map_rgba(170, 170, 170, 200),
			al_map_rgba(150,   0,   0, 200));
}

rect_t ui_mkrect(short x, short y, unsigned short w, unsigned short h)
{
	return (rect_t){x, y, w, h};
}

static int rect_hitxy(rect_t r, int x, int y)
{
	return x > r.x
		&& y > r.y
		&& x <= r.x + r.w
		&& y <= r.y + r.h;
}

static int rect_hit(rect_t r)
{
	return rect_hitxy(r, uistate.mousex, uistate.mousey);
}

static void rect_draw(rect_t r, ALLEGRO_COLOR c)
{
	GLubyte red, green, blue, alpha;

	al_unmap_rgba(c, &red, &green, &blue, &alpha);
	glColor4ub(red, green, blue, alpha);

	glBindTexture(GL_TEXTURE_2D, 0);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glVertex2f(r.x,     r.y+r.h);
		glVertex2f(r.x+r.w, r.y+r.h);
		glVertex2f(r.x+r.w, r.y    );
		glVertex2f(r.x,     r.y    );
	glEnd();
	glPopMatrix();
}

void rect_drawtext(rect_t r, ALLEGRO_COLOR c, const char *s)
{
	int i, len;
	int sx, fw = uistyle.fontw;
	int sy, fh = uistyle.fonth;

	if (s == NULL) return;

	len = strlen(s);
	/* make text and rect with the same center. */

	/* Sbegin = Rcenter - Ssize */
	sx = (r.x + r.w/2) - (fw * len)/2;
	sy = (r.y + r.h/2) - (fh)/2;

	rect_draw(r, c);

	glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(uistyle.font12));
	for (i=0; i<len; i++) drawchar(sx+i*fw, sy, s[i]);
}

static void drawchar(int x, int y, unsigned char c)
{
	int fw = uistyle.fontw;
	int fh = uistyle.fonth;

	float x0, x1, y0, y1;
	float scalex = 1.0/(95.0); /* number of characters in the bitmap
					horizontal axis, only one char in
					height, so scaley = 1. */

	glColor3ub(255, 255, 255);

	if (c < 32 || c >= 127) c = 32; /* 32 is ' ' */

	x0 = scalex*(c - 32);
	x1 = x0 + scalex;
	y0 = 0;
	y1 = 1;

	glBegin(GL_QUADS);
	glTexCoord2f(x0, y0); glVertex2i(x,	y+fh);
	glTexCoord2f(x1, y0); glVertex2i(x+fw,	y+fh);
	glTexCoord2f(x1, y1); glVertex2i(x+fw,	y   );
	glTexCoord2f(x0, y1); glVertex2i(x,	y   );
	glEnd();
}


/* Widgets */

int ui_buttonl(int id, rect_t r, const char *s)
{
	rect_t pressedrect = ui_mkrect(r.x+2, r.y+2, r.w, r.h);
	rect_t kbdrect = ui_mkrect(r.x-3, r.y-3, r.w+6, r.h+6);
	if (rect_hit(r)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}

	if (uistate.kbditem == 0)
		uistate.kbditem = id;

	if (uistate.kbditem == id)
		rect_drawtext(kbdrect, uistyle.kbdfocus, s);

	rect_drawtext(pressedrect, uistyle.shadow, s);

	if (uistate.hotitem == id) {
		if (uistate.activeitem == id)
			rect_drawtext(pressedrect, uistyle.active, s);
		else
			rect_drawtext(r, uistyle.hot, s);
	} else
		rect_drawtext(r, uistyle.away, s);

	if (uistate.kbditem == id) {
		if (uistate.keyentered == ALLEGRO_KEY_ENTER) {
			uistate.keyentered = 0;
			return 1;
		}
		if (uistate.keyentered == ALLEGRO_KEY_TAB) {
			uistate.keyentered = 0;
			uistate.kbditem = 0;
			/* keymod is not working. ALLEGRO problem. */
			if (uistate.keymod & ALLEGRO_KEYMOD_SHIFT)
				uistate.kbditem = uistate.lastwidget;
		}
	}
	uistate.lastwidget = id;

	return uistate.mousedown == 0
		&& uistate.hotitem == id
		&& uistate.activeitem == id;
}

int ui_button(int id, rect_t r)
{
	return ui_buttonl(id, r, NULL);
}

int ui_drag(int id, rect_t r, int *dx, int *dy)
{
	static int dragid; /* drag flag */

	if (dragid == id || rect_hit(r)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}

	if (uistate.hotitem == id) {
		if (uistate.activeitem == id) {
			int x = uistate.mousex - r.w / 2;
			int y = uistate.mousey - r.h / 2;
			rect_t atmouse = ui_mkrect(x, y, r.w, r.h);

			rect_draw(atmouse, uistyle.active);
			dragid = id;
			uistate.dragid = id;
		} else
			rect_draw(r, uistyle.hot);
	} else
		rect_draw(r, uistyle.away);

	if (uistate.mousedown == 0
			&& uistate.hotitem == id
			&& uistate.activeitem == id) {
		*dx = uistate.mousex;
		*dy = uistate.mousey;
		dragid = 0;
		return uistate.dragid;
	}
	return 0;
}

int ui_drop(int id, rect_t r, int dx, int dy)
{
	int dragid;
	if (rect_hit(r) && uistate.dragid)
		rect_draw(r, uistyle.active);
	else
		rect_draw(r, uistyle.away);

	if (!rect_hitxy(r, dx, dy))
		return 0;
	dragid = uistate.dragid;
	uistate.dragid = 0;
	return dragid;
}

void ui_label(int id, rect_t r, const char *s)
{
	rect_drawtext(r, uistyle.away, s);
}

int ui_textfield(int id, rect_t r, char *buf, size_t bufsz)
{
	int len = strlen(buf);
	int changed = 0;

	rect_t kbdrect = ui_mkrect(r.x-3, r.y-3, r.w+6, r.h+6);

	if (rect_hit(r)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}

	if (uistate.kbditem == 0) uistate.kbditem = id;
	if (uistate.kbditem == id) rect_draw(kbdrect, uistyle.kbdfocus);

	if (uistate.activeitem == id || uistate.hotitem == id)
		rect_draw(r, uistyle.active);
	else
		rect_draw(r, uistyle.away);
	buf[len] = '|';
	rect_drawtext(r, uistyle.away, buf);
	buf[len] = 0;

	if (uistate.kbditem == id) {
		switch (uistate.keyentered) {
		case ALLEGRO_KEY_TAB:
			uistate.kbditem = 0;
			uistate.keyentered = 0;
			if (uistate.keymod & ALLEGRO_KEYMOD_SHIFT)
				uistate.kbditem = uistate.lastwidget;
			break;
		case ALLEGRO_KEY_BACKSPACE:
			if (len <= 0) break;
			len--;
			buf[len] = 0;
			changed = 1;
			break;
		case ALLEGRO_KEY_ENTER:
			changed = 2;
		default :
			break;
		}
		if (uistate.keychar >= 32 && uistate.keychar < 128
				&& len < bufsz) {
			buf[len] = uistate.keychar;
			len++;
			buf[len] = 0;
			changed = 1;
		}
	}

	if (uistate.mousedown == 0
			&& uistate.hotitem == id
			&& uistate.activeitem == id)
		uistate.kbditem = id;
	uistate.lastwidget = id;
	return changed;
}
