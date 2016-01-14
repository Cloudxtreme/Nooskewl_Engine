#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/spell.h"
#include "Nooskewl_Engine/tokenizer.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Spell::Spell(std::string id) :
	id(id)
{
	load(id);
}

Spell::Spell()
{
}

std::string Spell::to_string()
{
	return string_printf("%s,%s,%d,%s",
		id.c_str(),
		name.c_str(),
		cost,
		get_ingredients_string().c_str()
	);
}

void Spell::from_string(std::string s)
{
	Tokenizer t(s, ',');

	id = t.next();

	name = t.next();

	cost = atoi(t.next().c_str());

	std::string ingredients_s = t.next();
	ingredients.clear();
	std::string s2;
	Tokenizer t2(ingredients_s, ':');

	while ((s2 = t2.next()) != "") {
		ingredients.push_back(s2);
	}
}

bool Spell::load(std::string id)
{
	XML *xml = new XML("spells/" + id + ".xml");

	std::list<XML *> &nodes = xml->get_nodes();

	std::list<XML *>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		XML *x = *it;
		std::string tag = x->get_name();
		handle_tag(x);
	}

	delete xml;

	return true;
}

Spell *Spell::clone()
{
	Spell *spell = new Spell();

	spell->id = id;
	spell->name = name;
	spell->cost = cost;
	spell->ingredients = ingredients;

	return spell;
}

void Spell::handle_tag(XML *xml)
{
	std::string tag = xml->get_name();

	if (tag == "name") {
		this->name = xml->get_value();
	}
	else if (tag == "cost") {
		this->cost = atoi(xml->get_value().c_str());
	}
	else if (tag == "ingredients") {
		std::string ingredients_s = xml->get_value();
		Tokenizer t(ingredients_s, ':');
		std::string s;
		while ((s = t.next()) != "") {
			ingredients.push_back(s);
		}
	}
}

std::string Spell::get_id()
{
	return id;
}

std::string Spell::get_name()
{
	return name;
}

int Spell::get_cost()
{
	return cost;
}

std::vector<std::string> &Spell::get_ingredients()
{
	return ingredients;
}

std::string Spell::get_ingredients_string()
{
	std::string s;

	for (size_t i = 0; i < ingredients.size(); i++) {
		if (i != 0) {
			s += ":";
		}
		s += ingredients[i];
	}

	return s;
}
