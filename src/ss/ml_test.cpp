#include "ss/ml_test.h"
#include "ss/tweens.h"

void SS_ML_Test::trigger(Map *map, Map_Entity *entity)
{
	Point<int> collide_pos;
	if (entity->get_id() == 0 && entity->collides(Point<int>(7, 1), Size<int>(2, 1), collide_pos)) {
		entity->stop();
		map->change_map("test2.map", Point<int>(7, 18) + collide_pos, N);
	}
}