#include "ss/ml_test2.h"
#include "ss/tweens.h"

void SS_ML_Test2::start(Map *map)
{
	test = new Map_Entity(NULL);
	test->load_sprite("player");
	test->set_position(Point<int>(10, 3));
	map->add_entity(test);
	DLList<Tween *> *t = new DLList<Tween *>();
	SS_Move_Tween *point1 = new SS_Move_Tween;
	point1->entity = test;
	point1->start_pos = test->get_position();
	point1->destination = Point<int>(10, 15);
	point1->speed = 25.0f;
	SS_Move_Tween *point2 = new SS_Move_Tween;
	point2->entity = test;
	point2->start_pos = Point<int>(10, 15);
	point2->destination = test->get_position();
	point2->speed = 25.0f;
	DLList_Node<Tween *> *node1 = t->push_back(point1);
	DLList_Node<Tween *> *node2 = t->push_back(point2);
	node2->next = node1;
	node1->prev = node2;
	tweens.push_back(t);
}

void SS_ML_Test2::end(Map *map)
{
	destroy_tweens(tweens);
}

void SS_ML_Test2::trigger(Map *map, Map_Entity *entity)
{
	Point<int> collide_pos;
	if (entity->get_id() == 0 && entity->collides(Point<int>(7, 18), Size<int>(2, 1), collide_pos)) {
		entity->stop();
		map->change_map("test.map", Point<int>(7, 1) + collide_pos, S);
	}
}

void SS_ML_Test2::update(Map *map)
{
	update_tweens(tweens);
}