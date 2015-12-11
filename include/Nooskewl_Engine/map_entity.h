#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Brain;
class Map;
class Sprite;
class Stats;

class NOOSKEWL_ENGINE_EXPORT Map_Entity {
public:
	static void new_game_started();
	static void end_sleep_callback(void *data);

	enum Type {
		NPC,
		OTHER
	};

	Map_Entity(std::string name);
	~Map_Entity();

	void set_brain(Brain *brain);
	void load_sprite(std::string name);
	void set_sprite(Sprite *sprite);
	void set_position(Point<int> position);
	void set_size(Size<int> size);
	void set_offset(Point<float> offset);
	void set_draw_offset(Point<int> draw_offset);
	void set_direction(Direction direction);
	void set_solid(bool solid);
	void set_sitting(bool sitting);
	void set_sleeping(bool sleeping);
	void set_path(std::list<A_Star::Node *> path, Callback callback = NULL, void *callback_data = NULL);
	void set_input_enabled(bool enabled);
	void set_type(Type type);
	void load_stats(std::string name);
	void set_stats(Stats *stats);
	void set_low(bool low);
	void set_high(bool high);
	void set_z(int z);
	void set_z_add(int z_add);
	void set_pre_sit_sleep_direction(Direction direction);
	void set_sit_sleep_directions(int sit_sleep_directions);
	void set_activate_next_tile(bool onoff);
	void set_name(std::string name);
	void set_stop_next_tile(bool stop_next_tile);
	void set_speed(float speed);
	void set_should_face_activator(bool should_face);

	int get_id();
	std::string get_name();
	Brain *get_brain();
	Sprite *get_sprite();
	Direction get_direction();
	Point<int> get_position();
	Point<float> get_offset();
	Point<int> get_draw_offset();
	Size<int> get_size();
	Point<float> get_draw_position();
	bool is_solid();
	bool is_sitting();
	bool is_sleeping();
	bool is_following_path();
	bool is_input_enabled();
	Type get_type();
	Stats *get_stats();
	bool is_low();
	bool is_high();
	int get_z();
	int get_z_add();
	bool is_moving();
	float get_speed();
	bool should_face_activator();
	bool can_cancel_astar();
	Direction get_pre_sit_sleep_direction();

	// Positions in pixels
	bool pixels_collide(Point<int> position, Size<int> size);
	// Positions in tiles
	bool tiles_collide(Point<int> position, Size<int> size, Point<int> &collide_pos);
	bool entity_collides(Map_Entity *entity);

	void stop();
	void handle_event(TGUI_Event *event);
	void update_stats();
	// return false to destroy
	bool update(bool can_move);
	// draws with z values
	void draw(Point<float> draw_pos, bool use_depth_buffer = true, bool sitting_n = false);

	bool save(std::string &out);

private:
	bool maybe_move();
	void stop_now();
	void follow_path();
	void end_a_star();
	void set_next_blink();
	void set_sitting_sleeping(bool sitting, bool onoff);

	int id;
	std::string name;
	Direction direction;
	Sprite *sprite;
	Brain *brain;
	Point<int> position;
	bool moving;
	float speed;
	Point<float> offset; // offset from tile when moving between tiles
	Point<int> draw_offset; // added to position when drawing
	bool solid;
	Size<int> size;
	bool stop_next_tile;
	bool activate_next_tile;
	bool sitting;
	bool sleeping;
	bool input_enabled;

	bool following_path;
	std::list<A_Star::Node *> path;
	Callback path_callback;
	void *path_callback_data;

	bool has_blink;
	float eye_colour[4];
	float blink_colour[4];
	Uint32 next_blink;

	Type type;
	Stats *stats;

	bool low;
	bool high;
	int z;
	int z_add;

	Direction pre_sit_sleep_direction;

	int sit_sleep_directions;
	bool sat;

	bool should_face; // should face player/character when talked to/activated

	int path_count;
};

} // End namespace Nooskewl_Engine

#endif // MAP_ENTITY_H
