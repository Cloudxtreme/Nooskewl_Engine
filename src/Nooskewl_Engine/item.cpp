#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Item::Item(std::string name)
{
	defaults();
	load(name);
}


void Item::use(Stats *stats)
{
}

bool Item::save(SDL_RWops *file)
{
	SDL_fprintf(file, "item=%s,condition:%d,weight:%d,min_attack:%d,max_attack:%d,min_defense:%d,max_defense:%d,min_value:%d,max_value:%d\n", name.c_str(), condition, weight, min_attack, max_attack, min_defense, max_defense, min_value, max_value);
	return true;
}

void Item::defaults()
{
	name =  "Unknown";
	condition = 0xffff / 2;
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
		else if (tag == "condition") {
			XML *x = xml->find("randn");
			if (x != 0) {
				Uint32 ret;
				XML_Helpers::handle_randn_tag(x, ret);
				condition = ret;
			}
			else {
				condition = atoi(xml->get_value().c_str());
			}
		}
		else if (tag == "weight") {
			XML *x = xml->find("randn");
			if (x != 0) {
				Uint32 ret;
				XML_Helpers::handle_randn_tag(x, ret);
				weight = ret;
			}
			else {
				weight = atoi(xml->get_value().c_str());
			}
		}
		else if (tag == "attack") {
			std::string min;
			std::string max;
			XML_Helpers::handle_min_max_tag(xml, min, max);
			min_attack = atoi(min.c_str());
			max_attack = atoi(max.c_str());
		}
		else if (tag == "defense") {
			std::string min;
			std::string max;
			XML_Helpers::handle_min_max_tag(xml, min, max);
			min_defense = atoi(min.c_str());
			max_defense = atoi(max.c_str());
		}
		else if (tag == "value") {
			std::string min;
			std::string max;
			XML_Helpers::handle_min_max_tag(xml, min, max);
			min_value = atoi(min.c_str());
			max_value = atoi(max.c_str());
		}
	}
}
