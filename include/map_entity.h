#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "brain.h"
#include "sprite.h"
#include "types.h"

class Map;

class Map_Entity {
public:
	Map_Entity(Brain *brain);
	~Map_Entity();

	void load_sprite(std::string name) throw (Error);
	void set_position(Point<int> position);
	void set_offset(Point<float> offset);
	void set_bounce(int bounce);
	void set_direction(Direction direction);

	int get_id();
	Direction get_direction();
	Point<int> get_position();
	Size<int> get_size();
	Point<int> get_draw_position();
	bool collides(Point<int> position, Size<int> size, Point<int> &collide_pos);

	void stop();
	void handle_event(TGUI_Event *event);
	// return false to destroy
	bool update(Map *map);
	void draw(Point<int> draw_pos);

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
};

#endif // MAP_ENTITY_H