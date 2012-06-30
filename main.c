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

static void opengl_init(void)
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* texture to gl */
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void game_init(unsigned int w,
		unsigned int h,
		unsigned int fps,
		unsigned int flags)
{
	assert(al_init());

	al_set_new_display_flags(ALLEGRO_OPENGL|ALLEGRO_RESIZABLE);
	assert((G.display = al_create_display(w, h)));
	opengl_init();

	assert((G.q = al_create_event_queue()));
	assert((G.tick = al_create_timer(1.0 / fps)));
	assert(al_init_image_addon());
	assert(al_install_mouse());
	assert(al_install_keyboard());

	ui_setstyle_default();
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

char character_name[21] = "name";

void ui_render(void)
{
	int i, dragbase;
	int x, y;

	static int toggle[4];
	const char *opts[4] = {
		"items",
		"equips",
		"stats",
		"skills",
	};

	ui_begin(G.w, G.h);

	switch (ui_textfield(50, ui_mkrect(150, 32, 150, 16),
				character_name, 20)) {
	case 2: printf("%s\n", character_name); break;
	case 1: printf("changed\n"); break;
	}

	for (i=0; i<4; i++) {
		if (ui_buttonl(i+1, ui_mkrect(5, 17*i+5, 64, 16), opts[i])) {
			toggle[i] = !toggle[i];
			printf("%d %s -> %d\n", i, opts[i], toggle[i]);
		}
	}

	if (toggle[0]) {
		for (i=0; i<21; i++) {
			int posx = 150+33*(i%3);
			int posy = 350+33*(i/3);
			int dragv;

			dragv = ui_drag(100+i, ui_mkrect(posx, posy, 32, 32), &x, &y);
			if (dragv) {
				dragbase = 100;
				printf("dragv: %d\n", dragv);
			}
		}

		for (i=0; i<21; i++) {
			int drop=0;
			int posx = 350+33*(i%3);
			int posy = 350+33*(i/3);

			drop = ui_drop(200+i, ui_mkrect(posx, posy, 32, 32), x, y);
			if (drop)
				printf("[ui_drop]: (%d,%d) %d -> %d\n",
						x, y, drop-dragbase, i);
		}
	}
	ui_end();
}

void game_render(void)
{
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
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
			al_acknowledge_resize(ev.display.source);

			G.w = ev.display.width;
			G.h = ev.display.height;

			printf("resize to: %d %d\n", G.w, G.h);
			break;
		}

		if (redraw && al_event_queue_is_empty(G.q)){
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			game_render();
			ui_render();
			al_flip_display();

			redraw=0;
		}
	}
}

void game_end(void)
{
	al_destroy_display(G.display);
	al_destroy_timer(G.tick);
	al_destroy_event_queue(G.q);
	ui_finish();
}

int main(int argc, const char *argv[])
{
	game_init(800, 600, 60, 0);
	game_loop();
	game_end();
	return 0;
}
