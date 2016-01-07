#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/translation.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Stats::Stats()
{
	defaults();
}

Stats::Stats(std::string name)
{
	defaults();

	load(name);

	try {
		profile_pic = new Image("profile_pics/" + name + ".tga", true);
	}
	catch (FileNotFoundError e) {
		if (sex == FEMALE) {
			profile_pic = new Image("profile_pics/female.tga", true);
		}
		else if (sex == MALE) {
			profile_pic = new Image("profile_pics/male.tga", true);
		}
		else {
			profile_pic = new Image("profile_pics/unknown.tga", true);
		}
	}
}

Stats::~Stats()
{
	delete profile_pic;
	delete inventory;
}

void Stats::defaults()
{
	name = "Unknown";

	profile_pic = 0;

	status = NORMAL;

	alignment = NEUTRAL;

	sex = UNKNOWN;

	hp = 0;
	mp = 0;

	characteristics.set_max_hp(0);
	characteristics.set_max_mp(0);
	characteristics.set_attack(0);
	characteristics.set_defense(0);
	characteristics.set_agility(0);
	characteristics.set_luck(0);
	characteristics.set_speed(0);
	characteristics.set_strength(0);

	experience = 0;

	karma = 0xffff / 2;
	hunger = 0;
	thirst = 0;
	rest = 0xffff;
	sobriety = 0xffff;

	int current_time = noo.get_play_time();

	status_start = current_time;
	ate_time = current_time;
	drank_time = current_time;
	rested_time = current_time;
	used_time = current_time;

	weapon_index = -1;
	armour_index = -1;

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

	delete xml;

	return true;
}

void Stats::set_status(Status status)
{
	if (status == SICK && this->status != SICK) {
		noo.add_notification(noo.t->translate(19));
	}
	else if (status == DRUNK && this->status != DRUNK) {
		noo.add_notification(noo.t->translate(18));
		noo.add_notification(noo.t->translate(20));
		karma -= MIN(500, karma);
	}
	this->status = status;
	status_start = noo.get_play_time();
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
		else if (tag == "status") {
			status = (Status)XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "alignment") {
			std::string value = xml->get_value();
			if (value == "good") {
				alignment = GOOD;
			}
			else if (value == "evil") {
				alignment = EVIL;
			}
			else {
				alignment = NEUTRAL;
			}
		}
		else if (tag == "sex") {
			std::string value = xml->get_value();
			if (value == "male") {
				sex = MALE;
			}
			else if (value == "female") {
				sex = FEMALE;
			}
			else {
				sex = UNKNOWN;
			}
		}
		else if (tag == "weapon") {
			weapon_index = atoi(xml->get_value().c_str());
		}
		else if (tag == "armour") {
			armour_index = atoi(xml->get_value().c_str());
		}
		else if (tag == "hp") {
			hp = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "max_hp") {
			characteristics.set_max_hp(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mp") {
			mp = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "max_mp") {
			characteristics.set_max_mp(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "attack") {
			characteristics.set_attack(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "defense") {
			characteristics.set_defense(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "agility") {
			characteristics.set_agility(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "luck") {
			characteristics.set_luck(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "speed") {
			characteristics.set_speed(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "strength") {
			characteristics.set_strength(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "experience") {
			experience = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "karma") {
			karma = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "hunger") {
			hunger = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "thirst") {
			thirst = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "rest") {
			rest = XML_Helpers::handle_numeric_tag(xml);
		}
		else if (tag == "sobriety") {
			sobriety = XML_Helpers::handle_numeric_tag(xml);
		}
	}
}
