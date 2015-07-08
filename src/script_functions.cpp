#include "dllist.h"
#include "map_entity.h"
#include "script_functions.h"

struct Tween {
	bool (*func)(Tween *tween);
	void *data;
	bool started;
	Uint32 start_time;

	Tween(bool (*func)(Tween *tween), void *data) :
		func(func),
		data(data),
		started(false)
	{
	}

	~Tween()
	{
		delete data;
	}
};

/* MISC */
static bool collides(Map_Entity *entity, Point<int> position, Size<int> size, Point<int> &collide_pos)
{
	Point<int> p = entity->get_position();
	if (p.x >= position.x && p.x < position.x+size.w && p.y >= position.y && p.y < position.y+size.h) {
		collide_pos = Point<int>(p.x-position.x, p.y-position.y);
		return true;
	}
	return false;
}

/* TWEENS */

struct move_tween_data {
	Map_Entity *entity;
	Point<int> start_pos;
	Point<int> destination;
	Uint32 start_time;
	float speed; // pixels/second
};

static bool move_tween(Tween *tween)
{
	move_tween_data *data = (move_tween_data *)tween->data;

	Uint32 now = SDL_GetTicks();
	Uint32 elapsed = now - tween->start_time;
	float elapsed_f = (float)elapsed / 1000.0f;

	int pixels_moved = int(elapsed_f * data->speed);

	Point<int> dist = data->destination - data->start_pos;
	Size<int> dist_pixels = Size<int>(dist.x*8, dist.y*8);

	if (dist_pixels.length() <= pixels_moved) {
		data->entity->set_position(data->destination);
		data->entity->set_offset(Point<float>(0.0f, 0.0f));
		return false;
	}

	float angle = dist_pixels.angle();

	Point<int> new_pos = Point<int>(int(cos(angle) * pixels_moved), int(sin(angle) * pixels_moved)) + data->start_pos * 8;
	Point<int> new_tile = new_pos / 8;
	Point<int> offset_pix = new_pos - (new_tile * 8);
	Point<float> offset = offset_pix;
	offset /= 8.0f;

	data->entity->set_position(new_tile);
	data->entity->set_offset(offset);

	return true;
}

/* SCRIPT FUNCS */
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
	else if (event_type == SCRIPT_END) {
		/*
		delete data;;
		for (size_t i = 0; i < tweens.size(); i++) {
			delete tweens[i];
		}
		*/
	}
}

void sf_test2(Script_Update event_type, Map *map, void **sd, void *data2)
{
	struct sf_test2_data {
		std::vector<DLList *> tweens;
		Map_Entity *test;
	} *data = (sf_test2_data *)*sd;

	std::vector<DLList *> *tweens;

	if (data) {
		tweens = &data->tweens;
	}

	if (event_type == SCRIPT_BEGIN) {
		*sd = new sf_test2_data;
		data = (sf_test2_data *)*sd;
		data->test = new Map_Entity(NULL);
		data->test->load_sprite("player");
		data->test->set_position(Point<int>(10, 3));
		map->add_entity(data->test);
		DLList *t = new DLList();
		move_tween_data *point1 = new move_tween_data;
		point1->entity = data->test;
		point1->start_pos = data->test->get_position();
		point1->destination = Point<int>(10, 15);
		point1->speed = 25.0f;
		move_tween_data *point2 = new move_tween_data;
		point2->entity = data->test;
		point2->start_pos = Point<int>(10, 15);
		point2->destination = data->test->get_position();
		point2->speed = 25.0f;
		DLList_Node *node1 = t->push_back(new Tween(move_tween, point1));
		DLList_Node *node2 = t->push_back(new Tween(move_tween, point2));
		node2->next = node1;
		node1->prev = node2;
		data->tweens.push_back(t);
	}
	else if (event_type == SCRIPT_TRIGGERS) {
		Map_Entity *e = (Map_Entity *)data2;
		Point<int> collide_pos;
		if (e->get_id() == 0 && collides(e, Point<int>(7, 18), Size<int>(2, 1), collide_pos)) {
			e->stop();
			map->change_map("test.map", Point<int>(7, 1) + collide_pos, S);
		}
	}
	else if (event_type == SCRIPT_UPDATE) {
		std::vector<DLList *>::iterator it;
		for (it = tweens->begin(); it != tweens->end();) {
			DLList *list = *it;
			Tween *t = (Tween *)list->nodes->data;
			if (t) {
				if (t->started == false) {
					t->started = true;
					t->start_time = SDL_GetTicks();
				}
				if (t->func(t) == false) {
					t->started = false;
					list->nodes = list->nodes->next;
					if (list->nodes == NULL) {
						delete list;
						it = tweens->erase(it);
						continue;
					}
				}
			}
			it++;
		}
	}
	else if (event_type == SCRIPT_END) {
		delete data;;
		for (size_t i = 0; i < tweens->size(); i++) {
			delete (*tweens)[i];
		}
	}
}