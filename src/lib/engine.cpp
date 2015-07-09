#include "Nooskewl_Engine/Nooskewl_Engine.h"

SDL_Joystick *joy;

void init_nooskewl_engine(int argc, char **argv)
{
	load_dll();

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		throw Error("SDL_Init failed");
	}

	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
	}

	cpa = new CPA();

	init_audio();
	init_video(argc, argv);
	init_font();
	init_graphics();
}

void shutdown_nooskewl_engine()
{
	shutdown_graphics();
	shutdown_font();
	shutdown_video();
	shutdown_audio();

	delete cpa;

	if (SDL_JoystickGetAttached(joy)) {
		SDL_JoystickClose(joy);
	}	
}