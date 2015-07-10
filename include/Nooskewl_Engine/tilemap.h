#ifndef TILEMAP_H
#define TILEMAP_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Tilemap
{
public:
	Tilemap(int tile_size, std::string map_filename) throw (Error);
	~Tilemap();

	int get_layer_count();
	int get_width();
	int get_height();
	int get_tile_size();

	bool is_solid(int layer, Point<int> position);
	bool collides(int layer, Point<int> topleft, Point<int> bottomright);

	void draw(int layer, Point<int> position);

private:
	struct Layer
	{
		int **sheets;
		int **tiles;
		bool **solids;
		std::vector<int> sheets_used;
	};

	int tile_size; // in pixels
	std::vector<Image *> sheets;

	std::vector<int> *sheets_used;

	int width; // in tiles
	int height; // in tiles
	int num_layers;

	Layer *layers;
};

} // End namespace Nooskewl_Engine

#endif // TILEMAP_H