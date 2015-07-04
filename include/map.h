#ifndef MAP_H
#define MAP_H

#include "map_entity.h"
#include "tilemap.h"

class Map {
public:
	Map();
	~Map();

	bool load(std::string name);
	void add_entity(Map_Entity *entity);

	bool is_solid(int layer, Point<int> position);

	void handle_event(SDL_Event *event);
	bool update();
	void draw();

private:
	Tilemap *tilemap;
	Point<float> offset;
	std::vector<Map_Entity *> entities;
};

#endif // MAP_H