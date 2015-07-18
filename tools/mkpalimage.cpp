#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: %s <input.gpl> <output.png>\n", argv[0]);
		return 0;
	}

	struct RGB {
		unsigned char r, g, b;
	} palette[256];

	memset(palette, 0, 256*sizeof(RGB));

	FILE *f = fopen(argv[1], "r");
	char buf[1000];
	int count = 0;

	while (count < 256 && fgets(buf, 1000, f)) {
		int r, g, b;
		if (sscanf(buf, "%d %d %d ", &r, &g, &b)) {
			palette[count].r = r;
			palette[count].g = g;
			palette[count].b = b;
			count++;
		}
	}

	al_init();
	al_init_image_addon();

	BITMAP *bitmap = al_create_bitmap(256, 1);

	for (int c = 0; c < 256; c++) {
		al_put_pixel(c, 0, al_map_rgb(palette[c].r, palette[c].g, palette[c].b));
	}

	al_save_bitmap(argv[2], bitmap);
}