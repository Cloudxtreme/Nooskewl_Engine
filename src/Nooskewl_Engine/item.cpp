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
		x = handle_rand_tag(x);
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
				handle_randn_tag(x, ret);
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
				handle_randn_tag(x, ret);
				weight = ret;
			}
			else {
				weight = atoi(xml->get_value().c_str());
			}
		}
		else if (tag == "attack") {
			std::string min;
			std::string max;
			handle_min_max_tag(xml, min, max);
			min_attack = atoi(min.c_str());
			max_attack = atoi(max.c_str());
		}
		else if (tag == "defense") {
			std::string min;
			std::string max;
			handle_min_max_tag(xml, min, max);
			min_defense = atoi(min.c_str());
			max_defense = atoi(max.c_str());
		}
		else if (tag == "value") {
			std::string min;
			std::string max;
			handle_min_max_tag(xml, min, max);
			min_value = atoi(min.c_str());
			max_value = atoi(max.c_str());
		}
	}
}

XML *Item::handle_rand_tag(XML *xml)
{
	std::list<XML *> &nodes = xml->get_nodes();

	std::list<XML *>::iterator it;

	int total_percent = 0;
	std::vector< std::pair<int, XML *> > values;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		int percent;
		XML *x = *it;
		XML *value = x->find("value");
		if (value == 0) {
			continue;
		}
		XML *percent_xml = x->find("percent");
		if (percent_xml) {
			percent = atoi(percent_xml->get_value().c_str());
		}
		else {
			percent = 100;
		}
		total_percent += percent;
		values.push_back(std::pair<int, XML *>(percent, value));
	}

	int r = rand() % total_percent;
	int percent = 0;

	for (size_t i = 0; i < values.size(); i++) {
		std::pair<int, XML *> &p = values[i];
		percent += p.first;
		if (r < percent) {
			return *(p.second->get_nodes().begin());
		}
	}

	return 0;
}

void Item::handle_min_max_tag(XML *xml, std::string &min, std::string &max)
{
	min = "0";
	max = "0";

	XML *min_xml = xml->find("min");
	XML *max_xml = xml->find("max");

	if (min_xml == 0 || max_xml == 0) {
		return;
	}

	XML *x = min_xml->find("rand");
	if (x == 0) {
		x = min_xml->find("randn");
		if (x == 0) {
			min = min_xml->get_value().c_str();
		}
		else {
			Uint32 ret;
			handle_randn_tag(x, ret);
			min = itos(ret);
		}
	}
	else {
		x = handle_rand_tag(x);
		min = x->get_value().c_str();
	}

	x = max_xml->find("rand");
	if (x == 0) {
		x = max_xml->find("randn");
		if (x == 0) {
			max = max_xml->get_value().c_str();
		}
		else {
			Uint32 ret;
			handle_randn_tag(x, ret);
			max = itos(ret);
		}
	}
	else {
		x = handle_rand_tag(x);
		max = x->get_value().c_str();
	}
}

void Item::handle_randn_tag(XML *xml, Uint32 &ret)
{
	ret = 0;

	XML *min_xml = xml->find("min");
	XML *max_xml = xml->find("max");

	if (min_xml == 0 || max_xml == 0) {
		return;
	}

	Uint32 min = atoi(min_xml->get_value().c_str());
	Uint32 max = atoi(max_xml->get_value().c_str());

	ret = (rand() % (max - min + 1)) + min;
}
