#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/tilemap.h"

using namespace Nooskewl_Engine;

Tilemap::Tilemap(int tile_size, std::string map_filename) :
	tile_size(tile_size)
{
	map_filename = "maps/" + map_filename;

	for (int i = 0; i < 256; i++) {
		std::string filename = std::string("sheets/tiles" + itos(i) + ".tga");
		Image *image;
		try {
			image = new Image(filename, true);
		}
		catch (Error e) {
			if (i == 0) {
				throw LoadError("no tile sheets!");
			}
			else {
				break;
			}
		}
		sheets.push_back(image);
	}

	SDL_RWops *f;
	try {
		f = open_file(map_filename);
	}
	catch (Error e) {
		for (size_t i = 0; i < sheets.size(); i++) {
			delete sheets[i];
		}
		sheets.clear();
		throw e;
	}

	width = SDL_ReadLE16(f);
	height = SDL_ReadLE16(f);
	num_layers = SDL_fgetc(f);

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
				layers[layer].tiles[row][col] = (int16_t)SDL_ReadLE16(f);
				layers[layer].sheets[row][col] = (char)SDL_fgetc(f);
				layers[layer].solids[row][col] = SDL_fgetc(f) != 0;

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
}

Tilemap::~Tilemap()
{
	for (size_t i = 0; i < sheets.size(); i++) {
		delete sheets[i];
	}

	if (layers) {
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

bool Tilemap::is_solid(int layer, Point<int> position)
{
	int start_layer = layer < 0 ? 0 : layer;
	int end_layer = layer < 0 ? num_layers - 1 : layer;

	for (int i = start_layer; i <= end_layer; i++) {
		Layer l = layers[i];
		if (l.solids[position.y][position.x]) {
			return true;
		}
	}

	return false;
}

bool Tilemap::collides(int layer, Point<int> topleft, Point<int> bottomright)
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

void Tilemap::draw(int layer, Point<int> position)
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
					int dx = position.x + col * tile_size;
					int dy = position.y + row * tile_size;
					sheets[s]->draw_region(
						Point<int>(sx, sy),
						Size<int>(tile_size, tile_size),
						Point<int>(dx, dy),
						0
					);
				}
			}
		}

		sheets[sheet_num]->end();
	}
}
