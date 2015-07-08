#ifndef MAP_H
#define MAP_H

#include "map_entity.h"
#include "speech.h"
#include "tilemap.h"
#include "types.h"

enum Script_Update {
	SCRIPT_BEGIN,
	SCRIPT_UPDATE,
	SCRIPT_TRIGGERS,
	SCRIPT_END
};

class Map;

typedef void (*Script_Func)(Script_Update event_type, Map *map, void **script_data, void *data2);

class Map {
public:
	Map(std::string map_name) throw (Error);
	~Map();

	void start();
	void end();

	void add_entity(Map_Entity *entity);
	void add_speeches(std::vector<std::string> &speeches);
	void change_map(std::string map_name, Point<int> position, Direction direction);

	bool is_solid(int layer, Point<int> position, Size<int> size);
	void check_triggers(Map_Entity *entity);
	void get_new_map_details(std::string &map_name, Point<int> &position, Direction &direction);
	Map_Entity *get_entity(int id);

	void handle_event(TGUI_Event *event);
	void update_camera();
	bool update();
	void draw();

private:
	void init(std::string map_name);

	Tilemap *tilemap;
	Point<int> offset;
	std::vector<Map_Entity *> entities;

	Script_Func script_func;
	void *script_data;

	std::vector<std::string> speeches;
	Speech *speech;

	std::string map_name;
	std::string new_map_name;
	Point<int> new_map_position;
	Direction new_map_direction;
};

#endif // MAP_H