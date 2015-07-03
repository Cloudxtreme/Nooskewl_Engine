#include "starsquatters.h"
#include "brain.h"

Brain::Brain() :
	l(false),
	r(false),
	u(false),
	d(false),
	b1(false)
{
}

Brain::~Brain()
{
}

Player_Brain::Player_Brain()
{
}

Player_Brain::~Player_Brain()
{
}

void Player_Brain::update(void *input)
{
	SDL_Event *event = (SDL_Event *)input;

	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
			case SDLK_LEFT:
				l = true;
				break;
			case SDLK_RIGHT:
				r = true;
				break;
			case SDLK_UP:
				u = true;
				break;
			case SDLK_DOWN:
				d = true;
				break;
			case SDLK_RETURN:
				b1 = true;
				break;
		}
	}
	else if (event->type == SDL_KEYUP) {
		switch (event->key.keysym.sym) {
			case SDLK_LEFT:
				l = false;
				break;
			case SDLK_RIGHT:
				r = false;
				break;
			case SDLK_UP:
				u = false;
				break;
			case SDLK_DOWN:
				d = false;
				break;
			case SDLK_RETURN:
				b1 = false;
				break;
		}
	}
}