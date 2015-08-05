#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Item::Item(std::string name)
{
	defaults();
	load(name);
}


void Item::use(Stats *stats)
{
}

bool Item::save(SDL_RWops *file)
{
	SDL_fprintf(file, "item=%s,condition:%d,weight:%d,attack:%d,defense:%d\n", name.c_str(), condition, weight, attack, defense);
	return true;
}

void Item::defaults()
{
	name =  "Unknown";
	condition = 0xffff / 2;
	weight = 0;
	min_attack = max_attack = 0;
	min_defense = max_defense = 0;
	min_value = max_value = 0;
}

bool Item::load(std::string name)
{
	return true;
}
