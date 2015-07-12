#include "Nooskewl_Engine/brain.h"

using namespace Nooskewl_Engine;

Brain::Brain()
{
	reset_input();
}

Brain::~Brain()
{
}

void Brain::reset_input()
{
	l = r = u = d = b1 = false;
}