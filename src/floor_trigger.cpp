#include "floor_trigger.h"

Floor_Trigger::Floor_Trigger(Point<int> topleft, Size<int> size, void (*function)(Map *map, Map_Entity *entity)) :
	topleft(topleft),
	size(size),
	function(function)
{
}

void ft_test(Map *map, Map_Entity *entity)
{
	printf("FLOOR TRIGGER!\n");
}