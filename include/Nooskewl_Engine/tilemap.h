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
	Tilemap(std::string map_filename);
	~Tilemap();

	int get_num_layers();
	int get_width();
	int get_height();

	// in tiles
	bool is_solid(int layer, Point<int> position);
	// in pixels
	bool collides(int layer, Point<int> topleft, Point<int> bottomright);

	void draw(int layer, Point<int> position, bool set_z = false);

private:
	struct Layer
	{
		int **sheet;
		int **x;
		int **y;
		bool **solid;
		std::vector<int> sheets_used;
	};

	std::vector<Image *> sheets;

	int width; // in tiles
	int height; // in tiles
	int num_layers;

	Layer *layers;
};

} // End namespace Nooskewl_Engine

#endif // TILEMAP_H