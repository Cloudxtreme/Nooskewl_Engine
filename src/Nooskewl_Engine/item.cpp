#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/tokenizer.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Item::Item(std::string name) :
	id(name)
{
	defaults();
	load(name);
}

Item::Item() :
	id("")
{
	defaults();
}

void Item::use(Stats *stats)
{
}

std::string Item::to_string()
{
	return string_printf("%s,%s,type:%d,condition:%d,weight:%d,min_attack:%d,max_attack:%d,min_defense:%d,max_defense:%d,min_value:%d,max_value:%d", id.c_str(), name.c_str(), (int)type, condition, weight, min_attack, max_attack, min_defense, max_defense, min_value, max_value);
}

void Item::from_string(std::string s)
{
	Tokenizer t(s, ',');

	id = t.next();

	name = t.next();

	std::string option;

	while ((option = t.next()) != "") {
		Tokenizer t2(option, ':');

		std::string key = t2.next();
		std::string value = t2.next();

		if (key == "type") {
			type = (Type)atoi(value.c_str());
		}
		else if (key == "condition") {
			condition = atoi(value.c_str());
		}
		else if (key == "weight") {
			weight = atoi(value.c_str());
		}
		else if (key == "min_attack") {
			min_attack = atoi(value.c_str());
		}
		else if (key == "max_attack") {
			max_attack = atoi(value.c_str());
		}
		else if (key == "min_defense") {
			min_defense = atoi(value.c_str());
		}
		else if (key == "max_defense") {
			max_defense = atoi(value.c_str());
		}
		else if (key == "min_value") {
			min_value = atoi(value.c_str());
		}
		else if (key == "max_value") {
			max_value = atoi(value.c_str());
		}
	}
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
	
	modifiers.set_max_hp(0);
	modifiers.set_max_mp(0);
	modifiers.set_attack(0);
	modifiers.set_defense(0);
	modifiers.set_agility(0);
	modifiers.set_luck(0);
	modifiers.set_speed(0);
	modifiers.set_strength(0);
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

	delete xml;

	return true;
}

int Item::get_value()
{
	int c = 100 * condition / 0xffff;
	return int(min_value + ((c / 100.0f) * (max_value - min_value)));
}

Item *Item::clone()
{
	Item *item = new Item();

	item->id = id;
	item->type = type;
	item->name = name;
	item->weight = weight;
	item->condition = condition;
	item->min_attack = min_attack;
	item->max_attack = max_attack;
	item->min_defense = min_defense;
	item->max_defense = max_defense;
	item->min_value = min_value;
	item->max_value = max_value;

	return item;
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
		else if (tag == "mod_max_hp") {
			modifiers.set_max_hp(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_max_mp") {
			modifiers.set_max_mp(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_attack") {
			modifiers.set_attack(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_defense") {
			modifiers.set_defense(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_agility") {
			modifiers.set_agility(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_luck") {
			modifiers.set_luck(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_speed") {
			modifiers.set_speed(XML_Helpers::handle_numeric_tag(xml));
		}
		else if (tag == "mod_strength") {
			modifiers.set_strength(XML_Helpers::handle_numeric_tag(xml));
		}
	}
}
