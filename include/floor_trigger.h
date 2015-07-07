#ifndef FLOOR_TRIGGER_H
#define FLOOR_TRIGGER_H

#include "map_entity.h"
#include "types.h"

class Floor_Trigger {
public:
	Point<int> topleft;
	Size<int> size;

	void (*function)(Map *map, Map_Entity *entity);

	Floor_Trigger(Point<int> topleft, Size<int> size, void (*function)(Map *map, Map_Entity *entity));
};

void ft_test(Map *map, Map_Entity *entity);
void ft_test2(Map *map, Map_Entity *entity);

#endif // FLOOR_TRIGGER_H