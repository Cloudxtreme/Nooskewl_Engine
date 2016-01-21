#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/tilemap.h"

using namespace Nooskewl_Engine;

Tilemap::Tilemap(std::string map_filename)
{
	map_filename = "maps/" + map_filename;

	for (int i = 0; i < 256; i++) {
		std::string filename = std::string("tiles/tiles" + itos(i) + ".tga");
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

	size.w = SDL_ReadLE16(f);
	size.h = SDL_ReadLE16(f);
	num_layers = SDL_fgetc(f);

	layers = new Layer[num_layers];

	for (int layer = 0; layer < num_layers; layer++) {
		layers[layer].groups = std::vector<Group *>();
		layers[layer].sheets_used = std::vector<int>();
		layers[layer].sheet = new int *[size.h];
		layers[layer].x = new int *[size.h];
		layers[layer].y = new int *[size.h];
		layers[layer].solid = new bool *[size.h];
		for (int row = 0; row < size.h; row++) {
			layers[layer].sheet[row] = new int[size.w];
			layers[layer].x[row] = new int[size.w];
			layers[layer].y[row] = new int[size.w];
			layers[layer].solid[row] = new bool[size.w];
			for (int col = 0; col < size.w; col++) {
				layers[layer].x[row][col] = (char)SDL_fgetc(f);
				layers[layer].y[row][col] = (char)SDL_fgetc(f);
				layers[layer].sheet[row][col] = (char)SDL_fgetc(f);
				layers[layer].solid[row][col] = SDL_fgetc(f) != 0;
				if (layers[layer].x[row][col] >= 0 && std::find(layers[layer].sheets_used.begin(), layers[layer].sheets_used.end(), layers[layer].sheet[row][col]) == layers[layer].sheets_used.end()) {
					layers[layer].sheets_used.push_back(layers[layer].sheet[row][col]);
				}
			}
		}
	}

	int num_groups = SDL_ReadLE16(f);

	for (int i = 0; i < num_groups; i++) {
		Group *g = new Group;
		g->type = SDL_ReadLE32(f);
		g->layer = SDL_fgetc(f);
		g->position.x = SDL_ReadLE16(f);
		g->position.y = SDL_ReadLE16(f);
		g->size.w = SDL_ReadLE16(f);
		g->size.h = SDL_ReadLE16(f);
		layers[g->layer].groups.push_back(g);
	}

	int num_walls = SDL_ReadLE16(f);

	for (int i = 0; i < num_walls; i++) {
		Wall *w = new Wall;
		w->position.x = SDL_ReadLE16(f);
		w->position.y = SDL_ReadLE16(f);
		w->position.z = SDL_ReadLE16(f);
		w->size.x = SDL_ReadLE16(f);
		w->size.y = SDL_ReadLE16(f);
		w->size.z = SDL_ReadLE16(f);
		walls.push_back(w);
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
			for (int row = 0; row < size.h; row++) {
				delete[] layers[layer].sheet[row];
				delete[] layers[layer].x[row];
				delete[] layers[layer].y[row];
				delete[] layers[layer].solid[row];
			}
			delete[] layers[layer].sheet;
			delete[] layers[layer].x;
			delete[] layers[layer].y;
			delete[] layers[layer].solid;
			for (size_t i = 0; i < layers[layer].groups.size(); i++) {
				delete layers[layer].groups[i];
			}
		}

		delete[] layers;
	}
}

int Tilemap::get_num_layers()
{
	return num_layers;
}

Size<int> Tilemap::get_size()
{
	return size;
}

bool Tilemap::is_solid(int layer, Point<int> position)
{
	if (position.x < 0 || position.y < 0 || position.x >= size.w || position.y >= size.h) {
		return true;
	}

	int start_layer = layer < 0 ? 0 : layer;
	int end_layer = layer < 0 ? num_layers - 1 : layer;

	for (int i = start_layer; i <= end_layer; i++) {
		Layer l = layers[i];
		if (l.solid[position.y][position.x]) {
			return true;
		}
	}

	return false;
}

bool Tilemap::collides(int layer, Point<int> topleft, Point<int> bottomright)
{
	int start_layer = layer < 0 ? 0 : layer;
	int end_layer = layer < 0 ? num_layers - 1 : layer;

	int start_column = topleft.x / noo.tile_size;
	int end_column = bottomright.x / noo.tile_size;
	int start_row = topleft.y / noo.tile_size;
	int end_row = bottomright.y / noo.tile_size;

	start_column = MIN(size.w-1, MAX(0, start_column));
	end_column = MIN(size.w-1, MAX(0, end_column));
	start_row = MIN(size.h-1, MAX(0, start_row));
	end_row = MIN(size.h-1, MAX(0, end_row));

	for (int i = start_layer; i <= end_layer; i++) {
		Layer l = layers[i];

		for (int row = start_row; row <= end_row; row++) {
			for (int column = start_column; column <= end_column; column++) {
				if (l.solid[row][column]) {
					return true;
				}
			}
		}
	}

	return false;
}

void Tilemap::draw(int layer, Point<float> position, bool use_depth_buffer)
{
	Layer l = layers[layer];

	for (size_t sheet = 0; sheet < l.sheets_used.size(); sheet++) {
		int sheet_num = l.sheets_used[sheet];

		sheets[sheet_num]->start();

		for (int row = 0; row < size.h; row++) {
			for (int col = 0; col < size.w; col++) {
				int s = l.sheet[row][col];
				if (s == sheet_num) {
					int x = l.x[row][col];
					int y = l.y[row][col];
					int sx = x * noo.tile_size;
					int sy = y * noo.tile_size;
					float dx = position.x + col * noo.tile_size;
					float dy = position.y + row * noo.tile_size;
					int dw = noo.tile_size;
					int dh = noo.tile_size;

					// Clipping
					if (dx < -noo.tile_size || dy < -noo.tile_size || dx >= noo.screen_size.w+noo.tile_size || dy >= noo.screen_size.h+noo.tile_size) {
						continue;
					}

					sheets[s]->draw_region_z(
						Point<int>(sx, sy),
						Size<int>(dw, dh),
						Point<float>(dx, dy),
						use_depth_buffer ? get_z(layer, col, row) : 0.0f,
						0
					);
				}
			}
		}

		sheets[sheet_num]->end();
	}
}

std::vector<Tilemap::Group *> Tilemap::get_groups(int layer)
{
	int start_layer = layer < 0 ? 0 : layer;
	int end_layer = layer < 0 ? num_layers - 1 : layer;

	std::vector<Group *> g;

	for (int i = start_layer; i <= end_layer; i++) {
		Layer l = layers[i];

		g.insert(g.end(), l.groups.begin(), l.groups.end());
	}

	return g;
}

float Tilemap::get_z(int layer, int x, int y)
{
	Layer l = layers[layer];
	for (size_t i = 0; i < l.groups.size(); i++) {
		Group *g = l.groups[i];
		if ((g->type & Group::GROUP_OBJECT) == 0) {
			continue;
		}
		if (x >= g->position.x && y >= g->position.y && x < (g->position.x+g->size.w) && y < (g->position.y+g->size.h)) {
			// We multiply by 0.01f so the map transition which is 3D keeps graphics on the same plane.
			// 0.01f is big enough that a 16 bit depth buffer still works and small enough it looks right
			return -(1.0f - ((float)((g->position.y + g->size.h - 1) * noo.tile_size) / (float)(size.h * noo.tile_size))) * 0.01f;
		}
	}
	return -(1.0f - ((float)(y * noo.tile_size) / (float)(size.h * noo.tile_size))) * 0.01f;
}
