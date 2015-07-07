#ifndef TGUI3_H
#define TGUI3_h

#ifdef _MSC_VER
#define TGUI3_FUNC __declspec(dllexport)
#else
#define TGUI3_FUNC
#endif

#ifdef WITH_SDL
#include <SDL.h>

TGUI3_FUNC void tgui_sdl_handle_event(SDL_Event *sdl_event);
#endif // WITH_SDL

enum TGUI_Event_Type {
	TGUI_UNKNOWN,
	TGUI_KEY_DOWN,
	TGUI_KEY_UP,
	TGUI_MOUSE_DOWN,
	TGUI_MOUSE_UP,
	TGUI_MOUSE_AXIS,
	TGUI_JOY_DOWN,
	TGUI_JOY_UP,
	TGUI_JOY_AXIS
};

struct TGUI_Event {
	TGUI_Event_Type type;
	struct TGUI_Event_Keyboard {
		int code;
	} keyboard;
	struct TGUI_Event_Mouse {
		int button;
		int x;
		int y;
	} mouse;
	struct TGUI_Event_Joystick {
		int button;
		int axis;
		float value; // -1.0 -> 1.0
	} joystick;
};

#endif // TGUI3_H