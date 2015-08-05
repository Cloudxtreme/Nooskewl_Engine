#ifndef STATS_H
#define STATS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Item;

class Stats {
public:
	enum Alignment {
		GOOD,
		NEUTRAL,
		EVIL
	};

	std::string name;

	Alignment alignment;

	Uint16 hp, max_hp;
	Uint16 mp, max_mp;
	Uint16 attack;
	Uint16 defense;
	Uint16 agility;
	Uint16 karma;
	Uint16 luck;
	Uint16 speed;
	Uint16 strength;
	Uint32 experience;

	Item *weapon;
	Item *armour;

	Inventory *inventory;

	Stats(std::string name);

	void defaults();
	bool load(std::string name);
};

} // End namespace Nooskewl_Engine

#endif // STATS_H
