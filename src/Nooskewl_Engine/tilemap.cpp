#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/tilemap.h"

using namespace Nooskewl_Engine;

// FIXME
	struct Light {
		Vec3D<float> position;
		SDL_Colour colour;
		float brightness;
	};
#include "Nooskewl_Engine/map_entity.h"


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

					SDL_Colour light;

					get_tile_lighting(Point<int>(col, row), light);

					// FIXME:
					light = noo.white;

					sheets[s]->draw_region_tinted_z(
						light,
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

// NOTE: code found here: http://forums.create.msdn.com/forums/t/280.aspx
static bool checkcoll_line_line(const Point<float> a1, const Point<float> a2, const Point<float> b1, const Point<float> b2, Point<float> *result)
{
	double Ua, Ub;

	Ua = ((b2.x - b1.x) * (a1.y - b1.y) - (b2.y - b1.y) * (a1.x - b1.x)) / ((b2.y - b1.y) * (a2.x - a1.x) - (b2.x - b1.x) * (a2.y - a1.y));

	Ub = ((a2.x - a1.x) * (a1.y - b1.y) - (a2.y - a1.y) * (a1.x - b1.x)) / ((b2.y - b1.y) * (a2.x - a1.x) - (b2.x - b1.x) * (a2.y - a1.y));

	if (Ua >= 0.0f && Ua <= 1.0f && Ub >= 0.0f && Ub <= 1.0f)
	{
		if (result) {
			result->x = a1.x + Ua * (a2.x - a1.x);
			result->y = a1.y + Ua * (a2.y - a1.y);
		}

		return true;
	}
	else
	{
		return false;
	}
}

static bool checkcoll_line_box(Point<float> a, Point<float> b, Point<float> topleft, Point<float> bottomright, Point<float> *result)
{
	Point<float> topright(
		bottomright.x,
		topleft.y
	);
	Point<float> bottomleft(
		topleft.x,
		bottomright.y
	);

	if (
		checkcoll_line_line(a, b, topleft, topright, result) ||
		checkcoll_line_line(a, b, topright, bottomright, result) ||
		checkcoll_line_line(a, b, bottomright, bottomleft, result) ||
		checkcoll_line_line(a, b, bottomleft, topleft, result))
	{
		return true;
	}

 	return false;
}

static bool checkcoll_line_wall(Point<float> tile_pos, Point<float> light_pos, Tilemap::Wall *w, Tilemap::Wall *tile_wall)
{
	bool is_face = tile_wall && (tile_pos.y >= tile_wall->position.y + tile_wall->size.y - tile_wall->size.z);
	if (w == tile_wall && is_face && tile_pos.y <= light_pos.y) {
		return false;
	}

	Point<int> topleft(w->position.x, w->position.y);
	Point<int> bottomright(w->position.x + w->size.x - 1, w->position.y + w->size.y - 1);

	if (checkcoll_line_box(tile_pos, light_pos, topleft, bottomright, 0)) {
		return true;
	}

	return false;
}

void Tilemap::get_tile_lighting(Point<int> tile_position, SDL_Colour &out)
{
	std::vector<Light> lights;

	Light l;
	l.position = Vec3D<int>(noo.player->get_position().x, noo.player->get_position().y, 1);
	l.colour = noo.white;
	l.brightness = 2;
	lights.push_back(l);

	int out_colour[3] = { 128, 128, 128 }; // FIXME: ambient lighting

	Wall *tile_wall = get_tile_wall(tile_position);

	if (tile_wall) {
		tile_position.y = tile_wall->position.y + tile_wall->size.y - 1;
	}

	for (size_t i = 0; i < lights.size(); i++) {
		Light *l = &lights[i];
		Point<float> light_pos(l->position.x, l->position.y);

		bool hits_wall = false;

		for (size_t j = 0; j < walls.size(); j++) {
			Wall *w = walls[j];

			if (w->position.z + w->size.z < l->position.z) {
				continue;
			}

			if (checkcoll_line_wall(tile_position, light_pos, w, tile_wall)) {
				hits_wall = true;
				break;
			}
		}

		if (hits_wall) {
			continue;
		}

		float distance_light_to_tile = (Vec3D<float>(tile_position.x, tile_position.y, 0) - l->position).length();

		float mul = distance_light_to_tile <= 0 ? 1.0f : MIN(1.0f, l->brightness / distance_light_to_tile);

		out_colour[0] += l->colour.r * mul;
		out_colour[1] += l->colour.g * mul;
		out_colour[2] += l->colour.b * mul;
	}

	out.r = MIN(255, out_colour[0] / lights.size());
	out.g = MIN(255, out_colour[1] / lights.size());
	out.b = MIN(255, out_colour[2] / lights.size());
	out.a = 255;
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

Tilemap::Wall *Tilemap::get_tile_wall(Point<int> tile_position)
{
	for (size_t i = 0; i < walls.size(); i++) {
		Wall *w = walls[i];
		Point<int> start(w->position.x, w->position.y - w->position.z - w->size.z);
		Point<int> end(start.x + w->size.x, w->position.y + w->size.y);
		if (tile_position.x >= start.x && tile_position.y >= start.y && tile_position.x < end.x && tile_position.y < end.y) {
			return w;
		}
	}

	return 0;
}
