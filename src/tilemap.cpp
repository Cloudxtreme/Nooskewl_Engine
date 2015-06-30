#include "starsquatters.h"
#include "log.h"
#include "tilemap.h"
#include "util.h"

Tilemap::Tilemap(int tile_size) :
	tile_size(tile_size)
{
}

Tilemap::~Tilemap()
{
	for (size_t i = 0; i < sheets.size(); i++) {
		delete sheets[i];
	}

	for (int layer = 0; layer < num_layers; layer++) {
		for (int row = 0; row < height; row++) {
			delete[] layers[layer].sheets[row];
			delete[] layers[layer].tiles[row];
			delete[] layers[layer].solids[row];
		}
		delete layers[layer].sheets;
		delete[] layers[layer].tiles;
		delete[] layers[layer].solids;
	}

	delete[] layers;
}

bool Tilemap::load(std::string sheet_directory, std::string level_filename)
{
	for (int i = 0; i < 256; i++) {
		std::string filename = std::string(sheet_directory + "/tiles" + itos(i) + ".tga");
		infomsg("Attempting to load tile sheet '%s'.\n", filename.c_str());
		SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "rb");
		Image *image = NULL;
		if (file) {
			infomsg("- Found!\n");
			image = new Image();
			if (image->load_tga(file)) {
				sheets.push_back(image);
			}
			else {
				errormsg("Problem loading sheet!");
				sheets.push_back(NULL);
			}
			SDL_RWclose(file);
		}
		else {
			infomsg("- Not found!\n");
			if (i == 0) {
				errormsg("No tile sheets!\n");
			}
			break;
		}
	}

	SDL_RWops *f = SDL_RWFromFile(level_filename.c_str(), "rb");
	if (f == NULL) {
		errormsg("Can't open level: %s\n", level_filename.c_str());
		for (size_t i = 0; i < sheets.size(); i++) {
			delete sheets[i];
		}
		sheets.clear();
		return false;
	}

	width = SDL_ReadLE32(f);
	height = SDL_ReadLE32(f);
	num_layers = SDL_ReadLE32(f);

	layers = new Layer[num_layers];

	for (int layer = 0; layer < num_layers; layer++) {
		layers[layer].sheets = new int *[height];
		layers[layer].tiles = new int *[height];
		layers[layer].solids = new bool *[height];
		layers[layer].sheets_used = std::vector<int>();
		for (int row = 0; row < height; row++) {
			layers[layer].sheets[row] = new int[width];
			layers[layer].tiles[row] = new int[width];
			layers[layer].solids[row] = new bool[width];
			for (int col = 0; col < width; col++) {
				layers[layer].tiles[row][col] = SDL_ReadLE32(f);
				layers[layer].sheets[row][col] = SDL_fgetc(f);
				layers[layer].solids[row][col] = SDL_fgetc(f);

				if (layers[layer].tiles[row][col] >= 0 && std::find(layers[layer].sheets_used.begin(), layers[layer].sheets_used.end(), layers[layer].sheets[row][col]) == layers[layer].sheets_used.end()) {
					layers[layer].sheets_used.push_back(layers[layer].sheets[row][col]);
				}
			}
		}
	}

	SDL_RWclose(f);

	for (int layer = 0; layer < num_layers; layer++) {
		std::sort(layers[layer].sheets_used.begin(), layers[layer].sheets_used.end());
	}

	return true;
}

int Tilemap::get_layer_count()
{
	return num_layers;
}

int Tilemap::get_width()
{
	return width;
}

int Tilemap::get_height()
{
	return height;
}

int Tilemap::get_tile_size()
{
	return tile_size;
}

bool Tilemap::collides(Point<float> topleft, Point<float> bottomright, int layer)
{
	int start_layer = layer < 0 ? 0 : layer;
	int end_layer = layer < 0 ? num_layers - 1 : layer;

	int start_column = topleft.x / tile_size;
	int end_column = bottomright.x / tile_size;
	int start_row = topleft.y / tile_size;
	int end_row = bottomright.y / tile_size;

	start_column = MIN(width-1, MAX(0, start_column));
	end_column = MIN(width-1, MAX(0, end_column));
	start_row = MIN(height-1, MAX(0, start_row));
	end_row = MIN(height-1, MAX(0, end_row));

	for (int i = start_layer; i <= end_layer; i++) {
		Layer l = layers[i];

		for (int row = start_row; row <= end_row; row++) {
			for (int column = start_column; column <= end_column; column++) {
				if (l.solids[row][column]) {
					return true;
				}
			}
		}
	}

	return false;
}

void Tilemap::draw_layer(int layer, float x, float y)
{
	Layer l = layers[layer];

	for (size_t sheet = 0; sheet < l.sheets_used.size(); sheet++) {
		int sheet_num = l.sheets_used[sheet];

		int width_in_tiles = sheets[sheet_num]->w / tile_size;

		sheets[sheet_num]->start();

		for (int row = 0; row < height; row++) {
			for (int col = 0; col < width; col++) {
				int s = l.sheets[row][col];
				if (s == sheet_num) {
					int tile = l.tiles[row][col];
					int sx = tile % width_in_tiles * tile_size;
					int sy = tile / width_in_tiles * tile_size;
					int dx = x + col * tile_size;
					int dy = y + row * tile_size;
					sheets[s]->draw_region(
						sx,
						sy,
						tile_size,
						tile_size,
						dx,
						dy,
						0
					);
				}
			}
		}

		sheets[sheet_num]->end();
	}
}