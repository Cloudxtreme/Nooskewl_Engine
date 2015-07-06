#ifndef MAP_H
#define MAP_H

#include "floor_trigger.h"
#include "map_entity.h"
#include "speech.h"
#include "tilemap.h"

class Map {
public:
	Map(std::string map_name) throw (Error);
	~Map();

	void add_entity(Map_Entity *entity);
	void add_speeches(std::vector<std::string> &speeches);

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

	std::vector<std::string> speeches;
	Speech *speech;
};

#endif // MAP_H