#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class Map;

class NOOSKEWL_ENGINE_EXPORT Map_Entity {
public:
	Map_Entity(Brain *brain);
	~Map_Entity();

	void stop();
	void handle_event(TGUI_Event *event);
	// return false to destroy
	bool update(Map *map, bool can_move);
	// draws with z values
	void draw(Map *map, Point<int> draw_pos);

	void load_sprite(std::string name);
	void set_position(Point<int> position);
	void set_offset(Point<float> offset);
	void set_bounce(int bounce);
	void set_direction(Direction direction);
	void set_solid(bool solid);
	void set_sitting(bool sitting);
	void disable_input();
	void enable_input();

	int get_id();
	Brain *get_brain();
	Sprite *get_sprite();
	Direction get_direction();
	Point<int> get_position();
	Point<float> get_offset();
	Size<int> get_size();
	Point<int> get_draw_position();
	bool is_solid();
	bool is_sitting();

	// Positions in pixels
	bool pixels_collide(Point<int> position, Size<int> size);
	// Positions in tiles
	bool tiles_collide(Point<int> position, Size<int> size, Point<int> &collide_pos);

private:
	bool maybe_move(Map *map);
	void stop_now();

	int id;
	Direction direction;
	Sprite *sprite;
	Brain *brain;
	Point<int> position;
	bool moving;
	float speed;
	Point<float> offset;
	int bounce;
	bool solid;
	Size<int> size;
	bool stop_next_tile;
	bool sitting;
	bool input_disabled;
};

} // End namespace Nooskewl_Engine

#endif // MAP_ENTITY_H