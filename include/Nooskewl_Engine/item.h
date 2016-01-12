#ifndef ITEM_H
#define ITEM_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/stats.h"

namespace Nooskewl_Engine {

class XML;

class NOOSKEWL_ENGINE_EXPORT Item {
public:
	enum Type {
		OTHER,
		WEAPON,
		ARMOUR
	};

	std::string id;

	std::string components; // ids separated by :

	Type type;
	std::string name;
	int16_t weight; // last two digits are after the decimal
	uint16_t condition;
	uint16_t min_attack, max_attack;
	uint16_t min_defense, max_defense;
	uint32_t min_value, max_value;

	Stats::Characteristics modifiers;

	Item(std::string name);
	Item();

	virtual void use(Stats *stats);

	void defaults();
	bool load(std::string name);
	std::string to_string();
	void from_string(std::string s);

	int get_value();
	Item *clone();

private:
	void handle_tag(XML *xml);
};

} // End namespace Nooskewl_Engine

#endif // ITEM_H
