#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include "animation_set.h"
#include "brain.h"
#include "types.h"

class Map;

class Map_Entity {
public:
	Map_Entity(Brain *brain);
	~Map_Entity();

	void set_map(Map *map);
	bool load_animation_set(std::string name);
	void set_position(Point<int> position);

	int get_id();
	Direction get_direction();
	Point<int> get_position();

	void handle_event(SDL_Event *event);
	// return false to destroy
	bool update();
	void draw();

private:
	bool maybe_move();

	int id;
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