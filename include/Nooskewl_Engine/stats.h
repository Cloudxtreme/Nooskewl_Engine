#ifndef STATS_H
#define STATS_H

#include "Nooskewl_Engine/main.h"

// Xlib.h #define's Status as int
#ifdef __linux__
#undef Status
#endif

namespace Nooskewl_Engine {

class Item;
class Image;
class Inventory;

class NOOSKEWL_ENGINE_EXPORT Stats {
public:
	enum Alignment {
		GOOD,
		NEUTRAL,
		EVIL
	};

	enum Sex {
		MALE,
		FEMALE,
		UNKNOWN
	};

	enum Status {
		NORMAL,
		SICK,
		DRUNK
	};

	struct Characteristics {
		uint16_t max_hp;
		uint16_t max_mp;
		uint16_t attack;
		uint16_t defense;
		uint16_t agility;
		uint16_t luck;
		uint16_t speed;
		uint16_t strength;
	};

	std::string name;

	Image *profile_pic;

	Status status;

	Alignment alignment;

	Sex sex;

	uint16_t hp;
	uint16_t mp;
	uint32_t experience;

	Characteristics characteristics;

	uint16_t karma;
	uint16_t hunger;
	uint16_t thirst;
	uint16_t rest;
	uint16_t sobriety;

	// Times
	int status_start;
	int ate_time;
	int drank_time;
	int rested_time;
	int used_time; // alcohol/drugs

	Inventory *inventory;

	int weapon_index; // indices into inventory
	int armour_index;

	Stats();
	Stats(std::string name);
	~Stats();

	void defaults();
	bool load(std::string name);

	void set_status(Status status);

private:
	void handle_tag(XML *xml);
};

} // End namespace Nooskewl_Engine

#endif // STATS_H
