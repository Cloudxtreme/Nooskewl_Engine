#ifndef MAP_H
#define MAP_H

#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/map_logic.h"
#include "Nooskewl_Engine/speech.h"
#include "Nooskewl_Engine/tilemap.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Map {
public:
	static const float PAN_BACK_SPEED;

	static void new_game_started();

	Map(std::string map_name);
	~Map();

	void start();
	void end();

	void handle_event(TGUI_Event *event);
	void update_camera();
	bool update();
	void draw(bool use_depth_buffer = true);

	// Try to activate as entity, returns true on success
	bool activate(Map_Entity *entity);

	void add_entity(Map_Entity *entity);
	void add_speech(std::string text, Callback callback = NULL, void *callback_data = NULL);
	void change_map(std::string map_name, Point<int> position, Direction direction);
	void set_panning(bool panning);
	void set_pan(Point<float> pan);

	bool is_solid(int layer, Point<int> position, Size<int> size, bool check_entities = true, bool check_tiles = true);
	void check_triggers(Map_Entity *entity);
	void get_new_map_details(std::string &map_name, Point<int> &position, Direction &direction);
	Map_Entity *get_entity(int id);
	std::string get_map_name();
	Tilemap *get_tilemap();
	Point<float> get_offset();
	std::list<A_Star::Node *> find_path(Point<int> start, Point<int> goal);

private:
	struct Map_Speech {
		std::string text;
		Callback callback;
		void *callback_data;

		Map_Speech(std::string text, Callback callback, void *callback_data) :
			text(text),
			callback(callback),
			callback_data(callback_data)
		{
		}
	};

	Tilemap *tilemap;
	Point<float> offset;
	bool panning;
	Point<float> pan;
	float pan_angle;
	std::vector<Map_Entity *> entities;

	std::vector<Map_Speech *> speeches;
	Speech *speech;

	std::string map_name;
	std::string new_map_name;
	Point<int> new_map_position;
	Direction new_map_direction;

	Map_Logic *ml;

	A_Star *a_star;
};

} // End namespace Nooskewl_Engine

#endif // MAP_H
