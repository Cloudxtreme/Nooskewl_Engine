#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Item::Item(std::string name) :
	id(name)
{
	defaults();
	load(name);
}


void Item::use(Stats *stats)
{
}

bool Item::save(SDL_RWops *file)
{
	SDL_fprintf(file, "item=%s,type:%d,condition:%d,weight:%d,min_attack:%d,max_attack:%d,min_defense:%d,max_defense:%d,min_value:%d,max_value:%d\n", name.c_str(), (int)type, condition, weight, min_attack, max_attack, min_defense, max_defense, min_value, max_value);
	return true;
}

void Item::defaults()
{
	type = OTHER;
	name =  "Unknown";
	condition = 0xffff;
	weight = 0;
	min_attack = max_attack = 0;
	min_defense = max_defense = 0;
	min_value = max_value = 0;
}

bool Item::load(std::string name)
{
	XML *xml = new XML("items/" + name + ".xml");

	std::list<XML *> &nodes = xml->get_nodes();

	std::list<XML *>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		XML *x = *it;
		std::string tag = x->get_name();
		handle_tag(x);
	}

	return true;
}

void Item::handle_tag(XML *xml)
{
	std::string tag = xml->get_name();
	int32_t min, max;

	XML *x = xml->find("rand");
	if (x != 0) {
		x = XML_Helpers::handle_rand_tag(x);
		if (x != 0) {
			handle_tag(x);
		}
	}
	else {
		if (tag == "name") {
			this->name = xml->get_value();
		}
		else if (tag == "type") {
			std::string value = xml->get_value();
			if (value == "weapon") {
				type = WEAPON;
			}
			else if (value == "armour") {
				type = ARMOUR;
			}
			else {
				type = OTHER;
			}
		}
		else if (tag == "condition") {
			condition = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "weight") {
			weight = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "attack") {
			XML_Helpers::handle_min_max_tag(xml, min, max);
			min_attack = min;
			max_attack = max;
		}
		else if (tag == "defense") {
			XML_Helpers::handle_min_max_tag(xml, min, max);
			min_defense = min;
			max_defense = max;
		}
		else if (tag == "value") {
			XML_Helpers::handle_min_max_tag(xml, min, max);
			min_value = min;
			max_value = max;
		}
	}
}
