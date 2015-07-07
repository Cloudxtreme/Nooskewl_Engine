#include "floor_trigger.h"
#include "map.h"

Floor_Trigger::Floor_Trigger(Point<int> topleft, Size<int> size, void (*function)(Map *map, Map_Entity *entity)) :
	topleft(topleft),
	size(size),
	function(function)
{
}

void ft_test(Map *map, Map_Entity *entity)
{
	map->change_map("test2.map", Point<int>(5, 5), N);
}

void ft_test2(Map *map, Map_Entity *entity)
{
	map->change_map("test.map", Point<int>(2, 5), N);
}