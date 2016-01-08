#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/tokenizer.h"
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
			Tokenizer t(xml->get_value(), ':');

			std::string s;

			while ((s = t.next()) != "") {
				weapon_indices.push_back(atoi(s.c_str()));
			}
		}
		else if (tag == "armour") {
			Tokenizer t(xml->get_value(), ':');

			std::string s;

			while ((s = t.next()) != "") {
				armour_indices.push_back(atoi(s.c_str()));
			}
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

Stats::Characteristics::Characteristics() :
	max_hp(0),
	max_mp(0),
	attack(0),
	defense(0),
	agility(0),
	luck(0),
	speed(0),
	strength(0)
{
}

Stats::Characteristics::Characteristics(const Characteristics &o)
{
	this->max_hp = o.max_hp;
	this->max_mp = o.max_mp;
	this->attack = o.attack;
	this->defense = o.defense;
	this->agility = o.agility;
	this->luck = o.luck;
	this->speed = o.speed;
	this->strength = o.strength;
}
	
uint16_t Stats::Characteristics::get_max_hp()
{
	return max_hp;
}

uint16_t Stats::Characteristics::get_max_mp()
{
	return max_mp;
}

uint16_t Stats::Characteristics::get_attack()
{
	return attack;
}

uint16_t Stats::Characteristics::get_defense()
{
	return defense;
}

uint16_t Stats::Characteristics::get_agility()
{
	return agility;
}

uint16_t Stats::Characteristics::get_luck()
{
	return luck;
}

uint16_t Stats::Characteristics::get_speed()
{
	return speed;
}

uint16_t Stats::Characteristics::get_strength()
{
	return strength;
}

uint16_t Stats::Characteristics::get_modified_max_hp(Stats *stats)
{
	return int16_t(max_hp + (max_hp * stats->get_max_hp_mod()));
}

uint16_t Stats::Characteristics::get_modified_max_mp(Stats *stats)
{
	return int16_t(max_mp + (max_mp * stats->get_max_mp_mod()));
}

uint16_t Stats::Characteristics::get_modified_attack(Stats *stats)
{
	return int16_t(attack + (attack * stats->get_attack_mod()));
}

uint16_t Stats::Characteristics::get_modified_defense(Stats *stats)
{
	return int16_t(defense + (defense * stats->get_defense_mod()));
}

uint16_t Stats::Characteristics::get_modified_agility(Stats *stats)
{
	return int16_t(agility + (agility * stats->get_agility_mod()));
}

uint16_t Stats::Characteristics::get_modified_luck(Stats *stats)
{
	return int16_t(luck + (luck * stats->get_luck_mod()));
}

uint16_t Stats::Characteristics::get_modified_speed(Stats *stats)
{
	return int16_t(speed + (speed * stats->get_speed_mod()));
}

uint16_t Stats::Characteristics::get_modified_strength(Stats *stats)
{
	return int16_t(strength + (strength * stats->get_strength_mod()));
}

void Stats::Characteristics::set_max_hp(uint16_t value)
{
	max_hp = value;
}

void Stats::Characteristics::set_max_mp(uint16_t value)
{
	max_mp = value;
}

void Stats::Characteristics::set_attack(uint16_t value)
{
	attack = value;
}

void Stats::Characteristics::set_defense(uint16_t value)
{
	defense = value;
}

void Stats::Characteristics::set_agility(uint16_t value)
{
	agility = value;
}

void Stats::Characteristics::set_luck(uint16_t value)
{
	luck = value;
}

void Stats::Characteristics::set_speed(uint16_t value)
{
	speed = value;
}

void Stats::Characteristics::set_strength(uint16_t value)
{
	strength = value;
}
	
Stats::Characteristics Stats::combine_equipment_modifiers()
{
	Characteristics c;

	c.set_max_hp(0xffff/2);
	c.set_max_mp(0xffff/2);
	c.set_attack(0xffff/2);
	c.set_defense(0xffff/2);
	c.set_agility(0xffff/2);
	c.set_luck(0xffff/2);
	c.set_speed(0xffff/2);
	c.set_strength(0xffff/2);

	for (size_t i = 0; i < weapon_indices.size(); i++) {
		Item *item = inventory->items[weapon_indices[i]][0];

		c.set_max_hp(MIN(0xffff, MAX(0, ((c.get_max_hp()-0xffff/2) + (item->modifiers.get_max_hp()-0xffff/2)) + (0xffff/2))));
		c.set_max_mp(MIN(0xffff, MAX(0, ((c.get_max_mp()-0xffff/2) + (item->modifiers.get_max_mp()-0xffff/2)) + (0xffff/2))));
		c.set_attack(MIN(0xffff, MAX(0, ((c.get_attack()-0xffff/2) + (item->modifiers.get_attack()-0xffff/2)) + (0xffff/2))));
		c.set_defense(MIN(0xffff, MAX(0, ((c.get_defense()-0xffff/2) + (item->modifiers.get_defense()-0xffff/2)) + (0xffff/2))));
		c.set_agility(MIN(0xffff, MAX(0, ((c.get_agility()-0xffff/2) + (item->modifiers.get_agility()-0xffff/2)) + (0xffff/2))));
		c.set_luck(MIN(0xffff, MAX(0, ((c.get_luck()-0xffff/2) + (item->modifiers.get_luck()-0xffff/2)) + (0xffff/2))));
		c.set_speed(MIN(0xffff, MAX(0, ((c.get_speed()-0xffff/2) + (item->modifiers.get_speed()-0xffff/2)) + (0xffff/2))));
		c.set_strength(MIN(0xffff, MAX(0, ((c.get_strength()-0xffff/2) + (item->modifiers.get_strength()-0xffff/2)) + (0xffff/2))));
	}

	for (size_t i = 0; i < armour_indices.size(); i++) {
		Item *item = inventory->items[armour_indices[i]][0];

		c.set_max_hp(MIN(0xffff, MAX(0, ((c.get_max_hp()-0xffff/2) + (item->modifiers.get_max_hp()-0xffff/2)) + (0xffff/2))));
		c.set_max_mp(MIN(0xffff, MAX(0, ((c.get_max_mp()-0xffff/2) + (item->modifiers.get_max_mp()-0xffff/2)) + (0xffff/2))));
		c.set_attack(MIN(0xffff, MAX(0, ((c.get_attack()-0xffff/2) + (item->modifiers.get_attack()-0xffff/2)) + (0xffff/2))));
		c.set_defense(MIN(0xffff, MAX(0, ((c.get_defense()-0xffff/2) + (item->modifiers.get_defense()-0xffff/2)) + (0xffff/2))));
		c.set_agility(MIN(0xffff, MAX(0, ((c.get_agility()-0xffff/2) + (item->modifiers.get_agility()-0xffff/2)) + (0xffff/2))));
		c.set_luck(MIN(0xffff, MAX(0, ((c.get_luck()-0xffff/2) + (item->modifiers.get_luck()-0xffff/2)) + (0xffff/2))));
		c.set_speed(MIN(0xffff, MAX(0, ((c.get_speed()-0xffff/2) + (item->modifiers.get_speed()-0xffff/2)) + (0xffff/2))));
		c.set_strength(MIN(0xffff, MAX(0, ((c.get_strength()-0xffff/2) + (item->modifiers.get_strength()-0xffff/2)) + (0xffff/2))));
	}

	return c;
}

float Stats::get_max_hp_mod()
{
	return (float)(combine_equipment_modifiers().get_max_hp() - 0xffff/2) / (0xffff/2);
}

float Stats::get_max_mp_mod()
{
	return (float)(combine_equipment_modifiers().get_max_mp() - 0xffff/2) / (0xffff/2);
}

float Stats::get_attack_mod()
{
	return (float)(combine_equipment_modifiers().get_attack() - 0xffff/2) / (0xffff/2);
}

float Stats::get_defense_mod()
{
	return (float)(combine_equipment_modifiers().get_defense() - 0xffff/2) / (0xffff/2);
}

float Stats::get_agility_mod()
{
	return (float)(combine_equipment_modifiers().get_agility() - 0xffff/2) / (0xffff/2);
}

float Stats::get_luck_mod()
{
	return (float)(combine_equipment_modifiers().get_luck() - 0xffff/2) / (0xffff/2);
}

float Stats::get_speed_mod()
{
	return (float)(combine_equipment_modifiers().get_speed() - 0xffff/2) / (0xffff/2);
}

float Stats::get_strength_mod()
{
	return (float)(combine_equipment_modifiers().get_strength() - 0xffff/2) / (0xffff/2);
}
