#ifndef INTERNAL_H
#define INTERNAL_H

#include "Nooskewl_Engine/error.h"

#ifdef NOOSKEWL_ENGINE_BUILD

using namespace Nooskewl_Engine;

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
int c99_snprintf(char* str, int size, const char* format, ...);

#endif

#endif INTERNAL_H