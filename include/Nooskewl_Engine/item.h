#ifndef ITEM_H
#define ITEM_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Stats;

class Item {
public:
	std::string name;
	Uint16 condition;
	Uint16 weight; // last two digits are after the decimal
	Uint16 attack;
	Uint16 defense;

	Item(std::string name);

	virtual void use(Stats *stats);
	virtual bool save(SDL_RWops *file);

	void defaults();
	bool load(std::string name);
};

} // End namespace Nooskewl_Engine

#endif // ITEM_H
