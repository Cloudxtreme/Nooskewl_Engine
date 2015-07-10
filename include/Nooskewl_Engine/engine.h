#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

NOOSKEWL_EXPORT void init_nooskewl_engine(int argc, char **argv);
NOOSKEWL_EXPORT bool update_nooskewl_engine();
NOOSKEWL_EXPORT void nooskewl_engine_handle_event(TGUI_Event *event);
NOOSKEWL_EXPORT void nooskewl_engine_draw();
NOOSKEWL_EXPORT void shutdown_nooskewl_engine();

} // End namespace Nooskewl_Engine

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // ENGINE_H