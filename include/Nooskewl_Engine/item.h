#ifndef ITEM_H
#define ITEM_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Stats;
class XML;

class Item {
public:
	std::string name;
	int16_t weight; // last two digits are after the decimal
	uint16_t condition;
	uint16_t min_attack, max_attack;
	uint16_t min_defense, max_defense;
	uint32_t min_value, max_value;

	Item(std::string name);

	virtual void use(Stats *stats);
	virtual bool save(SDL_RWops *file);

	void defaults();
	bool load(std::string name);

private:
	void handle_tag(XML *xml);
	XML *handle_rand_tag(XML *xml);
	void handle_min_max_tag(XML *xml, std::string &min, std::string &max);
	void handle_randn_tag(XML *xml, uint32_t &ret);
};

} // End namespace Nooskewl_Engine

#endif // ITEM_H
