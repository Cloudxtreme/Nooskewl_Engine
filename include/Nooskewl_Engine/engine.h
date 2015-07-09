#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"

EXPORT void init_nooskewl_engine(int argc, char **argv);
EXPORT bool update_nooskewl_engine();
EXPORT void nooskewl_engine_handle_event(TGUI_Event *event);
EXPORT void nooskewl_engine_draw();
EXPORT void shutdown_nooskewl_engine();

#endif // ENGINE_H