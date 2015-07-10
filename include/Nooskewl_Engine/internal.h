#ifndef INTERNAL_H
#define INTERNAL_H

#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/map.h"

#ifdef NOOSKEWL_ENGINE_BUILD

namespace Nooskewl_Engine {

typedef Map_Logic *(*Map_Logic_Getter)(Map *);

void load_dll();

/* MSVC doesn't have snprintf */

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
int c99_snprintf(char* str, int size, const char* format, ...);

} // End namespace Nooskewl_Engine

#endif

#endif INTERNAL_H