#ifndef ITEM_H
#define ITEM_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Stats;
class XML;

class Item {
public:
	std::string name;
	Uint16 weight; // last two digits are after the decimal
	Uint16 condition;
	Uint16 min_attack, max_attack;
	Uint16 min_defense, max_defense;
	Uint32 min_value, max_value;

	Item(std::string name);

	virtual void use(Stats *stats);
	virtual bool save(SDL_RWops *file);

	void defaults();
	bool load(std::string name);

private:
	void handle_tag(XML *xml);
	XML *handle_rand_tag(XML *xml);
	void handle_min_max_tag(XML *xml, std::string &min, std::string &max);
	void handle_randn_tag(XML *xml, Uint32 &ret);
};

} // End namespace Nooskewl_Engine

#endif // ITEM_H
