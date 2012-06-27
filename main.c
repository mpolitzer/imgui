#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_opengl.h>

#include "ui.h"

struct game {
	unsigned int w, h;

	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *tick;
	ALLEGRO_EVENT_QUEUE *q;
} G;

struct uielements {
	int slider;
	int drag_drop[8];
} uielements = {0, {1, 1, 1, 1, 1, 1, 1, 1}};

#define die(error, ...) _die(error, __FILE__, __LINE__, __VA_ARGS__)
void _die(int error, char* filename, int line, const char* format, ...)
{
	va_list args;

	fprintf(stderr, "[%s:%d] ", filename, line);
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(error);
}

static void opengl_init(void)
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, G.w, G.h, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* texture to gl */
	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);
}

void game_init(unsigned int w,
		unsigned int h,
		unsigned int fps,
		unsigned int flags)
{
	if (!al_init())
		die(1, "can't init allegro");

	al_set_new_display_flags(
			ALLEGRO_OPENGL
			| (flags ? ALLEGRO_FULLSCREEN : 0));
	if (!(G.display = al_create_display(w, h)))
		die(2, "can't create display");
	if (!(G.q = al_create_event_queue()))
		die(3, "can't create event queue");
	if (!(G.tick = al_create_timer(1.0 / fps)))
		die(4, "tick died, invalid fps");
	if (!al_init_image_addon())
		die(5, "failed to load image addon");
	if (!(al_install_mouse()))
		die(6, "can't init mouse");
	if (!(al_install_keyboard()))
		die(7, "can't init keyboard");
	opengl_init();

	G.w = w;
	G.h = h;

	al_register_event_source(G.q, al_get_mouse_event_source());
	al_register_event_source(G.q, al_get_keyboard_event_source());
	al_register_event_source(G.q, al_get_display_event_source(G.display));
	al_register_event_source(G.q, al_get_timer_event_source(G.tick));

	al_start_timer(G.tick);
}

int game_update(void)
{
	return 1;
}

void game_render(void)
{
	int i;
	static char v[10];
	int x, y;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ui_begin(G.w, G.h);

	if (ui_vslider(50, 300, 300, 100, &uielements.slider))
		printf("[ui_slider]: %d\n", uielements.slider);

	for (i=0; i<20; i++)
		ui_drag(400+i, 350+33*(i%3), 350+33*(i/3), 32, 32, &x, &y);
	for (i=0; i<20; i++) {
		int drop=0;
		drop = ui_drop(400+i, 550+33*(i%3), 350+33*(i/3), 32, 32, x, y);
		if (drop) {
			printf("[ui_drop]: (%d,%d) %d -> %d\n",
					x, y, drop-400, i);
		}
	}
	for (i=0; i<10; i++)
		if (ui_button(i+1, 100, 34*i+100, 100, 32)) {
			printf("submenu toggle\n");
			v[i] = ~v[i];
		}
	for (i=0; i<10; i++)
		if (v[i] && ui_button(i+11, 100+102, 34*i+100, 100, 32))
			printf("button %d pressed\n", i);
	if (ui_end())
		printf("ui!\n");

	al_flip_display();
}

void game_loop(void)
{
	int done=0;
	int redraw=1;

	while (!done){
		ALLEGRO_EVENT ev;

		al_wait_for_event(G.q, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER)
			redraw = game_update();

		ui_update(&ev);
		switch (ev.type) {
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			done=1;
			break;
		}

		if (redraw && al_event_queue_is_empty(G.q)){
			game_render();
			redraw=0;
		}
	}
}

void game_end(void)
{
}

int main(int argc, const char *argv[])
{
	game_init(800, 600, 60, 0);
	game_loop();
	game_end();
	return 0;
}
