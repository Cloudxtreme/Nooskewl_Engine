#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Stats::Stats(std::string name)
{
	defaults();

	load(name);

	printf("name=%s\nhp=%d\nmax_hp=%d\n", name.c_str(), hp, max_hp);
}

void Stats::defaults()
{
	name = "Unknown";

	alignment = NEUTRAL;

	hp = max_hp = 0;
	mp = max_mp = 0;

	attack = 0;
	defense = 0;
	agility = 0;
	karma = 0xffff / 2;
	luck = 0;
	speed = 0;
	strength = 0;
	experience = 0;

	weapon = 0;
	armour = 0;

	inventory = new Inventory();
}

bool Stats::load(std::string name)
{
	XML *xml = new XML("stats/" + name + ".xml");

	std::list<XML *> &nodes = xml->get_nodes();

	std::list<XML *>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		XML *x = *it;
		std::string tag = x->get_name();
		handle_tag(x);
	}

	return true;
}

void Stats::handle_tag(XML *xml)
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
		else if (tag == "hp") {
			hp = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "max_hp") {
			max_hp = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "mp") {
			mp = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "max_mp") {
			max_mp = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "attack") {
			attack = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "defense") {
			defense = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "agility") {
			agility = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "karma") {
			karma = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "luck") {
			luck = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "speed") {
			speed = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "strength") {
			strength = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "experience") {
			experience = XML_Helpers::handle_numeric_tag(xml);
		}
	}
}
