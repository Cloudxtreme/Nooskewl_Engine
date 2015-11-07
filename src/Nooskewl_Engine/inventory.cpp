#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/tokenizer.h"

using namespace Nooskewl_Engine;

static bool sort_items(const std::vector<Item *> &a, const std::vector<Item *> &b)
{
	return a[0]->name < b[0]->name;
}

std::string Inventory::decimal_to_string(int decimal)
{
	int tenths = decimal % 100;
	int hundredths = tenths % 10;
	tenths /= 10;
	int units = decimal / 100;
	std::string result = itos(units);
	if (tenths != 0 || hundredths != 0) {
		result += ".";
		result += itos(tenths);
		if (hundredths != 0) {
			result += itos(hundredths);
		}
	}
	return result;
}

Inventory::Inventory()
{
	gold = 0;
}

Inventory::~Inventory()
{
	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = 0; j < items[i].size(); j++) { 
			delete items[i][j];
		}
	}
}

void Inventory::add(Item *item)
{
	int index = find(item);

	if (index < 0) {
		std::vector<Item *> v;
		v.push_back(item);
		items.push_back(v);
	}
	else {
		items[index].push_back(item);
	}

	sort();
}

void Inventory::remove(Item *item)
{
	int index = find(item);
	if (index >= 0) {
		items[index].pop_back();
		if (items[index].size() == 0) {
			items.erase(items.begin() + index);
		}
	}
}

int Inventory::find(std::string id)
{
	int index = -1;

	for (size_t i = 0; i < items.size(); i++) {
		if (items[i].size() > 0) {
			Item *it = items[i][0];
			if (it->id == id) {
				index = i;
				break;
			}
		}
	}

	return index;
}

int Inventory::get_total_weight()
{
	int total_weight = 0;

	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = 0; j < items[i].size(); j++) {
			total_weight += items[i][j]->weight;
		}
	}

	return total_weight;
}

void Inventory::sort(int start, int end)
{
	std::vector< std::vector<Item *> >::iterator start_it;
	std::vector< std::vector<Item *> >::iterator end_it;

	if (start < 0) {
		start_it = items.begin();
		end_it = items.end();
	}
	else {
		start_it = items.begin() + start;
		if (end < 0 || end >= (int)items.size()) {
			end_it = items.end();
		}
		else {
			end_it = items.begin() + end;
		}
	}

	std::sort(start_it, end_it, sort_items);
}

std::string Inventory::to_string()
{
	std::string s = itos(gold) + "\n";

	s += itos(items.size()) + "\n";

	for (size_t i = 0; i < items.size(); i++) {
		s += itos(items[i].size()) + ",";
		s += items[i][0]->to_string() + "\n";
	}

	return s;
}

void Inventory::from_string(std::string s)
{
	// Destroy current items
	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = 0; j < items[i].size(); j++) {
			delete items[i][j];
		}
	}
	items.clear();

	Tokenizer t = Tokenizer(s, '\n');

	std::string gold_s = t.next();

	gold = atoi(gold_s.c_str());

	std::string num_s = t.next();

	int num = atoi(num_s.c_str());

	for (int i = 0; i < num; i++) {
		std::string line = t.next();

		Tokenizer t2(line, ',');

		std::string count_s = t2.next();
		int count = atoi(count_s.c_str());

		std::string item_s = line.substr(count_s.length()+1);
		trim(item_s);

		for (int i = 0; i < count; i++) {
			Item *item = new Item();
			item->from_string(item_s);
			add(item);
		}
	}
}

Inventory *Inventory::clone()
{
	Inventory *inventory = new Inventory();
	inventory->gold = gold;

	for (size_t i = 0; i < items.size(); i++) {
		inventory->items.push_back(std::vector<Item *>());
		for (size_t j = 0; j < items[i].size(); j++) {
			inventory->items[i].push_back(items[i][j]->clone());
		}
	}

	return inventory;
}

void Inventory::remove(Inventory *inventory)
{
	for (size_t i = 0; i < inventory->items.size(); i++) {
		for (size_t j = 0; j < inventory->items[i].size(); j++) {
			std::string id = inventory->items[i][j]->id;
			remove(id);
		}
	}
}

void Inventory::remove(std::string id)
{
	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = 0; j < items[i].size(); j++) {
			if (items[i][j]->id == id) {
				delete items[i][j];
				items[i].erase(items[i].begin() + j);
				if (items[i].size() == 0) {
					items.erase(items.begin() + i);
				}
				return;
			}
		}
	}
}

int Inventory::find(Item *item)
{
	int index = -1;

	for (size_t i = 0; i < items.size(); i++) {
		if (items[i].size() > 0) {
			Item *it = items[i][0];
			if (it->name == item->name) {
				index = i;
				break;
			}
		}
	}

	return index;
}
