#include "Nooskewl_Engine/basic_types.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/talk_brain.h"
#include "Nooskewl_Engine/tokenizer.h"

using namespace Nooskewl_Engine;

void Talk_Brain::callback(void *data)
{
	Callback_Data *d = (Callback_Data *)data;

	d->entity->set_direction(d->direction);

	if (d->user_callback) {
		d->user_callback(d->user_callback_data);
	}
}

Talk_Brain::Talk_Brain(std::string name, Callback callback, void *callback_data) :
	name(name),
	user_callback(callback),
	user_callback_data(callback_data)
{
	std::string text = noo.load_text("speech/" + name + ".utf8");

	int offset = 0;
	bool done = false;

	while (done == false) {
		while (text.c_str()[offset] != 0 && isspace(text.c_str()[offset])) {
			offset++;
		}
		if (text.c_str()[offset] == 0) {
			done = true;
		}

		int nlpos = text.find('\n', offset);
		int crpos = text.find('\r', offset);
		std::string line;

		if (nlpos == std::string::npos && crpos == std::string::npos) {
			done = true;
			line = text.substr(offset);
		}
		else {
			int less = MIN(nlpos, crpos);
			line = text.substr(offset, less-offset);
		}

		int pipepos = line.find('|');

		if (pipepos == std::string::npos) {
			printf("Malformated character speech file: %s.utf8\n", name.c_str());
		}
		else {
			std::string milestone_s = line.substr(0, pipepos);
			std::string speech = line.substr(pipepos+1);
			Talk *t = new Talk;
			t->milestone = atoi(milestone_s.c_str());
			t->text = speech;
			sayings.push_back(t);
		}

		offset = MAX(nlpos, crpos);
	}

	std::sort(sayings.begin(), sayings.end(), &Talk_Brain::compare_milestones);
}

Talk_Brain::~Talk_Brain()
{
	for (size_t i = 0; i < sayings.size(); i++) {
		delete sayings[i];
	}
}

void Talk_Brain::activate(Map_Entity *activator, Map_Entity *activated)
{
	for (size_t i = 0; i < sayings.size(); i++) {
		Talk *t = sayings[i];
		if (t->milestone < 0 || noo.check_milestone(t->milestone)) {
			callback_data.direction = activated->get_direction();
			callback_data.entity = activated;
			callback_data.user_callback = user_callback;
			callback_data.user_callback_data = user_callback_data;
			Point<int> offset = activator->get_position() - activated->get_position();
			if (offset.x < 0) {
				activated->set_direction(W);
			}
			else if (offset.x > 0) {
				activated->set_direction(E);
			}
			else if (offset.y < 0) {
				activated->set_direction(N);
			}
			else {
				activated->set_direction(S);
			}
			Point<float> entity_pos = (activated->get_position() + activated->get_offset()) * noo.tile_size + noo.map->get_offset();
			int pipe = t->text.find('|');
			if (pipe != std::string::npos) {
				std::string options = t->text.substr(0, pipe);
				Tokenizer tokenizer(options, ',');
				std::string s;
				bool x_set = false;
				bool y_set = false;
				while ((s = tokenizer.next()) != "") {
					if (s == "top" || s == "bottom") {
						y_set = true;
					}
					else if (s == "left" || s == "right") {
						x_set = true;
					}
				}
				if (x_set == false && entity_pos.x <= noo.screen_size.w/2) {
					t->text = std::string("right,") + t->text;
				}
				if (y_set == false && entity_pos.y > noo.screen_size.h/2) {
					t->text = std::string("top,") + t->text;
				}
			}
			noo.map->add_speech(t->text, callback, &callback_data);
			return;
		}
	}
}

bool Talk_Brain::compare_milestones(Talk *a, Talk *b)
{
	return a->milestone >= b->milestone;
}

bool Talk_Brain::save(SDL_RWops *file)
{
	SDL_fprintf(file, "brain=talk_brain,%s\n", name.c_str());
	return true;
}
