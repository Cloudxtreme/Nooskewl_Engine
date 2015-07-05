#ifndef MAP_H
#define MAP_H

#include "floor_trigger.h"
#include "map_entity.h"
#include "tilemap.h"

class Map {
public:
	Map();
	~Map();

	void load(std::string map_name) throw (Error);
	void add_entity(Map_Entity *entity);

	bool is_solid(int layer, Point<int> position);
	void check_triggers(Map_Entity *entity);

	void handle_event(SDL_Event *event);
	bool update();
	void draw();

private:
	void init_entities(std::string map_name);

	Tilemap *tilemap;
	Point<float> offset;
	std::vector<Map_Entity *> entities;
	std::vector<Floor_Trigger *> floor_triggers;
};

#endif // MAP_H