#include "floor_trigger.h"
#include "map.h"

Floor_Trigger::Floor_Trigger(Point<int> topleft, Size<int> size, void (*function)(Map *map, Map_Entity *entity, Point<int> touch_point)) :
	topleft(topleft),
	size(size),
	function(function)
{
}

void ft_test(Map *map, Map_Entity *entity, Point<int> touch_point)
{
	entity->stop();
	map->change_map("test2.map", Point<int>(7+touch_point.x, 18), N);
}

void ft_test2(Map *map, Map_Entity *entity, Point<int> touch_point)
{
	entity->stop();
	map->change_map("test.map", Point<int>(7+touch_point.x, 1), S);
}