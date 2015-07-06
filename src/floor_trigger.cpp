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
	entity->stop();
	std::vector<std::string> speeches;
	speeches.push_back("YO! TESTING!");
	speeches.push_back("We have received your goods and services tax/harmonized sales tax (GST/HST) return for the above-mentioned account for the period from January 1, 2011 to December 31, 2011. Because our records show that you already have a GST/HST return on file for this period, we did not process this duplicate record.");
	map->add_speeches(speeches);
}