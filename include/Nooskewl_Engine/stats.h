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
class XML;

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

	class Characteristics {
	public:
		uint16_t get_max_hp() { return max_hp; }
		uint16_t get_max_mp() { return max_mp; }
		uint16_t get_attack() { return attack; }
		uint16_t get_defense() { return defense; }
		uint16_t get_agility() { return agility; }
		uint16_t get_luck() { return luck; }
		uint16_t get_speed() { return speed; }
		uint16_t get_strength() { return strength; }

		uint16_t get_modified_max_hp(Stats *stats) { return max_hp + (max_hp * stats->get_max_hp_mod()); }
		uint16_t get_modified_max_mp(Stats *stats) { return max_mp + (max_mp * stats->get_max_mp_mod()); }
		uint16_t get_modified_attack(Stats *stats) { return attack + (attack * stats->get_attack_mod()); }
		uint16_t get_modified_defense(Stats *stats) { return defense + (defense * stats->get_defense_mod()); }
		uint16_t get_modified_agility(Stats *stats) { return agility + (agility * stats->get_agility_mod()); }
		uint16_t get_modified_luck(Stats *stats) { return luck + (luck * stats->get_luck_mod()); }
		uint16_t get_modified_speed(Stats *stats) { return speed + (speed * stats->get_speed_mod()); }
		uint16_t get_modified_strength(Stats *stats) { return strength + (strength * stats->get_strength_mod()); }

		void set_max_hp(uint16_t value) { max_hp = value; }
		void set_max_mp(uint16_t value) { max_mp = value; }
		void set_attack(uint16_t value) { attack = value; }
		void set_defense(uint16_t value) { defense = value; }
		void set_agility(uint16_t value) { agility = value; }
		void set_luck(uint16_t value) { luck = value; }
		void set_speed(uint16_t value) { speed = value; }
		void set_strength(uint16_t value) { strength = value; }

	private:
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

	float get_max_hp_mod() { return 1.0f; /* FIXME */ }
	float get_max_mp_mod() { return 1.0f; /* FIXME */ }
	float get_attack_mod() { return 1.0f; /* FIXME */ }
	float get_defense_mod() { return 1.0f; /* FIXME */ }
	float get_agility_mod() { return 1.0f; /* FIXME */ }
	float get_luck_mod() { return 1.0f; /* FIXME */ }
	float get_speed_mod() { return 1.0f; /* FIXME */ }
	float get_strength_mod() { return 1.0f; /* FIXME */ }

private:
	void handle_tag(XML *xml);
};

} // End namespace Nooskewl_Engine

#endif // STATS_H
