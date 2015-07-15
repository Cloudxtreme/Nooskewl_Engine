#include "Nooskewl_Engine/brain.h"

using namespace Nooskewl_Engine;

Brain::Brain()
{
	reset();
}

Brain::~Brain()
{
}

void Brain::reset()
{
	l = r = u = d = b1 = false;
}