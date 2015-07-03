#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "starsquatters.h"
#include "animation_set.h"
#include "brain.h"
#include "map.h"
#include "types.h"

class Map_Entity {
public:
	Map_Entity(Brain *brain);
	~Map_Entity();

	void set_map(Map *map);
	bool load_animation_set(std::string name);
	void set_position(Point<int> position);

	Direction get_direction();

	// return false to destroy
	bool update();
	void draw();

private:
	bool maybe_move();

	Direction direction;
	Map *map;
	Animation_Set *anim;
	Brain *brain;
	Point<int> position;
	bool moving;
	float speed;
	Point<float> offset;
};

#endif // MAP_ENTITY_H