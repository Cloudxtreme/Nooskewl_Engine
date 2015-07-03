#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "starsquatters.h"
#include "animation_set.h"
#include "brain.h"
#include "types.h"

class Map_Entity {
public:
	Map_Entity(Brain *brain);
	~Map_Entity();

	bool load_animation_set(std::string name);
	void set_position(Point<int> position);

	// return false to destroy
	bool update();
	void draw();

private:
	Brain *brain;
	Animation_Set *anim;
	Point<int> position;
	bool moving;
	Point<float> speed;
	Point<float> offset;
};

#endif // MAP_ENTITY_H