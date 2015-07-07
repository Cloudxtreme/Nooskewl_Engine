#include "script_functions.h"

static bool collides(Map_Entity *entity, Point<int> position, Size<int> size, Point<int> &collide_pos)
{
	Point<int> p = entity->get_position();
	if (p.x >= position.x && p.x < position.x+size.w && p.y >= position.y && p.y < position.y+size.h) {
		collide_pos = Point<int>(p.x-position.x, p.y-position.y);
		return true;
	}
	return false;
}

void sf_test(Script_Update event_type, Map *map, void **sd, void *data2)
{
	if (event_type == SCRIPT_TRIGGERS) {
		Map_Entity *e = (Map_Entity *)data2;
		Point<int> collide_pos;
		if (e->get_id() == 0 && collides(e, Point<int>(7, 1), Size<int>(2, 1), collide_pos)) {
			e->stop();
			map->change_map("test2.map", Point<int>(7, 18) + collide_pos, N);
		}
	}
}

void sf_test2(Script_Update event_type, Map *map, void **sd, void *data2)
{
	if (event_type == SCRIPT_TRIGGERS) {
		Map_Entity *e = (Map_Entity *)data2;
		Point<int> collide_pos;
		if (e->get_id() == 0 && collides(e, Point<int>(7, 18), Size<int>(2, 1), collide_pos)) {
			e->stop();
			map->change_map("test.map", Point<int>(7, 1) + collide_pos, S);
		}
	}	
}