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
	bool update(Map *map);
	void draw(Point<int> draw_pos);

	void load_sprite(std::string name) throw (Error);
	void set_position(Point<int> position);
	void set_offset(Point<float> offset);
	void set_bounce(int bounce);
	void set_direction(Direction direction);
	void set_solid(bool solid);

	int get_id();
	Direction get_direction();
	Point<int> get_position();
	Size<int> get_size();
	Point<int> get_draw_position();
	bool is_solid();
	// returns true if they overlap even if not solid so check is_solid()
	bool collides(Point<int> position, Size<int> size, Point<int> &collide_pos);

private:
	bool maybe_move(Map *map);

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
};

} // End namespace Nooskewl_Engine

#endif // MAP_ENTITY_H