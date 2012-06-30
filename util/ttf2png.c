#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include  <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

int main(int argc, char *argv[])
{
	ALLEGRO_FONT *in;
	ALLEGRO_BITMAP *out;
	char buffer[128];
	int i, ps, fw, fh;

	for (i=0; i<128; i++)
		buffer[i] = i;
	buffer[127] = 0;

	if (argc < 4) {
		fprintf(stderr, "usage: %s <file>.ttf <pointsize> <file2>.png\n",
				argv[0]);
		return 1;
	}
	sscanf(argv[2], " %d", &ps);

	assert(al_init());
	assert(al_init_image_addon());
	assert(al_init_ttf_addon());

	assert(in = al_load_ttf_font(argv[1], ps, 0));
	fw = al_get_text_width(in, &buffer[32]);
	fh = al_get_font_line_height(in);

	out = al_create_bitmap(fw, fh);
	al_set_target_bitmap(out);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	al_draw_text(in, al_map_rgb(0, 0, 0), 0, 0, 0, &buffer[32]);
	al_save_bitmap(argv[3], out);

	al_destroy_font(in);
	return 0;
}
