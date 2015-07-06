#ifndef MAP_H
#define MAP_H

#include "floor_trigger.h"
#include "map_entity.h"
#include "speech.h"
#include "tilemap.h"
#include "types.h"

class Map {
public:
	Map(std::string map_name) throw (Error);
	~Map();

	void add_entity(Map_Entity *entity);
	void add_speeches(std::vector<std::string> &speeches);
	void change_map(std::string map_name, Point<int> position, Direction direction);

	bool is_solid(int layer, Point<int> position);
	void check_triggers(Map_Entity *entity);
	void get_new_map_details(std::string &map_name, Point<int> &position, Direction &direction);

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

	std::string new_map_name;
	Point<int> new_map_position;
	Direction new_map_direction;
};

#endif // MAP_H