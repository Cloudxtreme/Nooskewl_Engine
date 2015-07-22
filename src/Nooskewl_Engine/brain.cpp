#include "Nooskewl_Engine/brain.h"

using namespace Nooskewl_Engine;

Brain::Brain()
{
	reset();
}

Brain::~Brain()
{
}

void Brain::handle_event(TGUI_Event *event)
{
}

void Brain::activate(Map_Entity *activator, Map_Entity *activated)
{
}

void Brain::reset()
{
	l = r = u = d = b1 = false;
}

bool Brain::save(SDL_RWops *file)
{
	SDL_fprintf(file, "brain=0\n");
	return true;
}