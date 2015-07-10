#ifndef LOAD_DLL_H
#define LOAD_DLL_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/map.h"

#ifdef NOOSKEWL_ENGINE_BUILD

namespace Nooskewl_Engine {

typedef Map_Logic *(*Map_Logic_Getter)(Map *);

void load_dll();

} // End namespace Nooskewl_Engine

#endif

#endif // LOAD_DLL_H