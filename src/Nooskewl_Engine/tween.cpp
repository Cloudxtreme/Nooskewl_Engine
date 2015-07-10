#include "Nooskewl_Engine/tween.h"

Tween::Tween() :
	started(false)
{
}

Tween::~Tween()
{
}

void Tween::update_tweens(Tweens tweens)
{
	std::vector< DLList<Tween *> *>::iterator it;
	for (it = tweens.begin(); it != tweens.end();) {
		DLList<Tween *> *list = *it;
		Tween *t = list->nodes->data;
		if (t) {
			if (t->started == false) {
				t->started = true;
				t->start_time = SDL_GetTicks();
			}
			if (t->update(t) == false) {
				t->started = false;
				list->nodes = list->nodes->next;
				if (list->nodes == NULL) {
					delete list;
					it = tweens.erase(it);
					continue;
				}
			}
		}
		it++;
	}
}

void Tween::destroy_tweens(Tweens tweens)
{
	for (size_t i = 0; i < tweens.size(); i++) {
		delete tweens[i];
	}
	tweens.clear();
}