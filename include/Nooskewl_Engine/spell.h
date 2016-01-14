#ifndef SPELL_H
#define SPELL_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class XML;

class NOOSKEWL_ENGINE_EXPORT Spell
{
public:
	Spell(std::string id);
	Spell();

	std::string get_id();
	std::string get_name();
	int get_cost();
	std::vector<std::string> &get_ingredients();
	std::string get_ingredients_string();
	std::string to_string();
	void from_string(std::string s);
	Spell *clone();

private:
	bool load(std::string id);
	void handle_tag(XML *xml);

	std::string id;
	std::string name;
	int cost;
	std::vector<std::string> ingredients;
};

} // End namespace Nooskewl_Engine

#endif // SPELL_H
