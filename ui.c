#include "ui.h"

struct uistate
{
	int mousex;
	int mousey;
	int mousedown;

	int hotitem;
	int activeitem;

	int kbditem;
	int keyentered;
	int keymod;

	int lastwidget;

	int dragid; /* store the ID that issued a ui_drag */
} uistate = {0,0,0,0,0};

static int regionhit(int x, int y, int w, int h)
{
	if (uistate.mousex < x ||
			uistate.mousey < y ||
			uistate.mousex >= x + w ||
			uistate.mousey >= y + h)
		return 0;
	return 1;
}

static void draw_rect(int x, int y, int w, int h, float r, float g, float b)
{
	glColor3f(r, g, b);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glVertex2f(x,   y+h);
		glVertex2f(x+w, y+h);
		glVertex2f(x+w, y  );
		glVertex2f(x,   y  );
	glEnd();
	glPopMatrix();
}

static void draw_rect_gray(int x, int y, int w, int h, float c)
{
	draw_rect(x, y, w, h, c, c, c);
}

void ui_update(ALLEGRO_EVENT *ev)
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
	}
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
	return (ui_state > 0) ? 1 : 0;
}

/* Widgets */

int ui_button(int id, int x, int y, int w, int h)
{
	if (regionhit(x, y, w, h)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}

	if (uistate.kbditem == 0)
		uistate.kbditem = id;
	if (uistate.kbditem == id)
		draw_rect(x-2, y-2, w+6, h+4, 0.5, 0, 0);

	draw_rect_gray(x+2, y+2, w, h, 0.5);

	if (uistate.hotitem == id)
	{
		if (uistate.activeitem == id)
			draw_rect_gray(x+2, y+2, w, h, 0.6);
		else
			draw_rect_gray(x, y, w, h, 0.7);
	} else {
		draw_rect_gray(x, y, w, h, 0.8);
	}

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

int ui_drag(int id, int x, int y, int w, int h, int *dx, int *dy)
{
	static int drag;
	if (drag || regionhit(x, y, w, h)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}


	if (uistate.hotitem == id)
	{
		if (uistate.activeitem == id) {
			draw_rect_gray(uistate.mousex-w/2,
					uistate.mousey-h/2, w, h, 0.5);
			drag = id;
		}
		else
			draw_rect_gray(x, y, w, h, 0.7);
	} else {
		draw_rect_gray(x, y, w, h, 0.8);
	}

	if (uistate.mousedown == 0
			&& uistate.hotitem == id
			&& uistate.activeitem == id) {
		*dx = uistate.mousex;
		*dy = uistate.mousey;
		drag = 0;
		uistate.dragid = id;
		return 1;
	}
	return 0;
}

int ui_drop(int id, int x, int y, int w, int h, int dx, int dy)
{
	draw_rect_gray(x, y, w, h, 0.8);
	if (dx < x ||
			dy < y ||
			dx >= x + w ||
			dy >= y + h)
		return 0;
	return uistate.dragid;
}

int ui_vslider(int id, int x, int y, int max, int *value)
{
	int ypos = ((256 - 16) * *value) / max;

	if (regionhit(x+8, y+8, 16, 255)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.mousedown)
			uistate.activeitem = id;
	}

	if (uistate.kbditem == 0)
		uistate.kbditem = id;
	if (uistate.kbditem == id)
		draw_rect(x-2, y-2, 32+4, 256+16+4, 0.5, 0, 0);

	draw_rect_gray(x, y, 32, 256+16, 0.1);

	if (uistate.activeitem == id || uistate.hotitem == id)
		draw_rect_gray(x+8, y+8 + ypos, 16, 16, 0.6);
	else
		draw_rect_gray(x+8, y+8 + ypos, 16, 16, 0.7);

	if (uistate.kbditem == id) {
		switch (uistate.keyentered) {
			float scale;
		case ALLEGRO_KEY_TAB:
			uistate.kbditem = 0;
			uistate.keyentered = 0;
			if (uistate.keymod & ALLEGRO_KEYMOD_SHIFT)
				uistate.kbditem = uistate.lastwidget;
			break;
		case ALLEGRO_KEY_UP:
			if (*value > 0) (*value)--;
			return 1;
		case ALLEGRO_KEY_DOWN:
			if (*value < max) (*value)++;
			return 1;
		case ALLEGRO_KEY_0:
		case ALLEGRO_KEY_1: case ALLEGRO_KEY_2: case ALLEGRO_KEY_3:
		case ALLEGRO_KEY_4: case ALLEGRO_KEY_5: case ALLEGRO_KEY_6:
		case ALLEGRO_KEY_7: case ALLEGRO_KEY_8: case ALLEGRO_KEY_9:
			scale = ((float)uistate.keyentered - ALLEGRO_KEY_0)/10;
			*value = max * (1 - scale);
			return 1;
		}
	}

	if (uistate.activeitem == id) {
		int v, mousepos;

		mousepos = uistate.mousey - (y + 8);
		if (mousepos < 0) mousepos = 0;
		if (mousepos > 255) mousepos = 255;

		v = (mousepos * max) / 255;
		if (v != *value) {
			*value = v;
			return 1;
		}
	}
	return 0;
}

