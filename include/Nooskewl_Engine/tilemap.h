#ifndef TILEMAP_H
#define TILEMAP_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Image;

class NOOSKEWL_ENGINE_EXPORT Tilemap
{
public:
	struct Group {
		// bit field flags
		enum Type {
			GROUP_NONE        = 0,
			GROUP_OBJECT      = 1,
			GROUP_CHAIR_NORTH = 1 << 1,
			GROUP_CHAIR_EAST  = 1 << 2,
			GROUP_CHAIR_SOUTH = 1 << 3,
			GROUP_CHAIR_WEST  = 1 << 4,
			GROUP_BED_NORTH = 1 << 5,
			GROUP_BED_EAST  = 1 << 6,
			GROUP_BED_SOUTH = 1 << 7,
			GROUP_BED_WEST  = 1 << 8
		};

		int type;
		int layer;
		Point<int> position;
		Size<int> size;
	};

	struct Wall {
		Vec3D<float> position;
		Vec3D<float> size;
	};

	Tilemap(std::string map_filename);
	~Tilemap();

	int get_num_layers();
	Size<int> get_size();

	// in tiles
	bool is_solid(int layer, Point<int> position);
	// in pixels
	bool collides(int layer, Point<int> topleft, Point<int> bottomright);

	void draw(int layer, Point<float> position, bool use_depth_buffer = true);

	std::vector<Group *> get_groups(int layer);

	// Following functions vertices ordered 0,0, 1,0, 1,1, 0,1 in screen coordinates

	void get_tile_lighting(Point<int> tile_position, SDL_Colour &out);

	void enable_lighting(bool enabled);
	void set_lighting_parameters(bool indoors, int outdoor_effect, SDL_Colour ambient_light);

private:
	float get_z(int layer, int x, int y);
	Wall *get_tile_wall(Point<int> tile_position);

	struct Layer
	{
		int **sheet;
		int **x;
		int **y;
		bool **solid;
		std::vector<Group *> groups;
		std::vector<int> sheets_used;
	};

	std::vector<Image *> sheets;

	Size<int> size; // in tiles
	int num_layers;

	Layer *layers;

	std::vector<Wall *> walls;

	// Lighting parameters
	bool lighting_enabled;
	bool indoors;
	int outdoor_effect; // % of total lighting effect to take from outdoors (0-100)
	SDL_Colour ambient_light;
};

} // End namespace Nooskewl_Engine

#endif // TILEMAP_H
