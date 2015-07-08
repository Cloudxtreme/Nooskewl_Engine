#include "ss/map_logic.h"

Map_Logic *ss_init_map_logic(Map *map)
{
	Map_Logic *ml = NULL;

	std::string map_name = map->get_map_name();

	if (map_name == "test.map") {
		ml = new SS_ML_Test();
	}
	else if (map_name == "test2.map") {
		ml = new SS_ML_Test2();
	}

	if (ml) {
		ml->start(map);
	}

	return ml;
}