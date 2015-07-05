#ifndef TILEMAP_H
#define TILEMAP_H

#include "error.h"
#include "image.h"
#include "types.h"

class Tilemap
{
public:
	Tilemap(int tile_size);
	~Tilemap();

	void load(std::string sheet_directory, std::string map_filename) throw (Error);

	int get_layer_count();
	int get_width();
	int get_height();
	int get_tile_size();

	bool is_solid(int layer, Point<int> position);
	bool collides(int layer, Point<float> topleft, Point<float> bottomright);

	void draw(int layer, float x, float y);

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

#endif // TILEMAP_H