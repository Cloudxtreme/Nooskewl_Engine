#include "Nooskewl_Engine/Nooskewl_Engine.h"

SDL_Joystick *joy;
bool mute;

void init_nooskewl_engine(int argc, char **argv)
{
	mute = check_args(argc, argv, "+mute");

	load_dll();

	int flags = SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO;
//	if (mute == false) {
		flags |= SDL_INIT_AUDIO;
//	}

	if (SDL_Init(flags) != 0) {
		throw Error("SDL_Init failed");
	}

	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
	}

	cpa = new CPA();

	init_audio(argc, argv);
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