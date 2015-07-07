#ifndef FLOOR_TRIGGER_H
#define FLOOR_TRIGGER_H

#include "map_entity.h"
#include "types.h"

class Floor_Trigger {
public:
	Point<int> topleft;
	Size<int> size;

	void (*function)(Map *map, Map_Entity *entity, Point<int> touch_point);

	Floor_Trigger(Point<int> topleft, Size<int> size, void (*function)(Map *map, Map_Entity *entity, Point<int> touch_point));
};

void ft_test(Map *map, Map_Entity *entity, Point<int> touch_point);
void ft_test2(Map *map, Map_Entity *entity, Point<int> touch_point);

#endif // FLOOR_TRIGGER_H