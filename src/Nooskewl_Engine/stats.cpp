#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Stats::Stats(std::string name)
{
	defaults();

	load(name);
}

void Stats::defaults()
{
	name = "Unknown";

	alignment = NEUTRAL;

	hp = max_hp = 0;
	mp = max_mp = 0;

	attack = 0;
	defense = 0;
	agility = 0;
	karma = 0xffff / 2;
	luck = 0;
	speed = 0;
	strength = 0;
	experience = 0;

	weapon = 0;
	armour = 0;

	inventory = new Inventory();
}

bool Stats::load(std::string name)
{
	return true;
}
