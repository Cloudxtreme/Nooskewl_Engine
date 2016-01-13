#ifndef STATS_H
#define STATS_H

// Xlib.h #define's Status as int
#ifdef __linux__
#undef Status
#endif

namespace Nooskewl_Engine {

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

	class NOOSKEWL_ENGINE_EXPORT Characteristics {
	public:
		Characteristics();
		Characteristics(const Characteristics &o);

		uint16_t get_max_hp();
		uint16_t get_max_mp();
		uint16_t get_attack();
		uint16_t get_defense();
		uint16_t get_agility();
		uint16_t get_luck();
		uint16_t get_speed();
		uint16_t get_strength();

		uint16_t get_modified_max_hp(Stats *stats);
		uint16_t get_modified_max_mp(Stats *stats);
		uint16_t get_modified_attack(Stats *stats);
		uint16_t get_modified_defense(Stats *stats);
		uint16_t get_modified_agility(Stats *stats);
		uint16_t get_modified_luck(Stats *stats);
		uint16_t get_modified_speed(Stats *stats);
		uint16_t get_modified_strength(Stats *stats);

		void set_max_hp(uint16_t value);
		void set_max_mp(uint16_t value);
		void set_attack(uint16_t value);
		void set_defense(uint16_t value);
		void set_agility(uint16_t value);
		void set_luck(uint16_t value);
		void set_speed(uint16_t value);
		void set_strength(uint16_t value);

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

	// indices into inventory
	int weapon_index;
	int armour_index;

	Stats();
	Stats(std::string name);
	~Stats();

	void defaults();
	bool load(std::string name);

	void set_status(Status status);

	Characteristics combine_equipment_modifiers();

	float get_max_hp_mod();
	float get_max_mp_mod();
	float get_attack_mod();
	float get_defense_mod();
	float get_agility_mod();
	float get_luck_mod();
	float get_speed_mod();
	float get_strength_mod();

private:
	void handle_tag(XML *xml);
};

} // End namespace Nooskewl_Engine

#endif // STATS_H
