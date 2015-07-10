#ifndef SNPRINTF_H
#define SNPRINTF_H

#ifdef _MSC_VER

#include "Nooskewl_Engine/main.h"

#ifdef NOOSKEWL_ENGINE_BUILD

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
int c99_snprintf(char* str, int size, const char* format, ...);

#endif

#endif // _MSC_VER

#endif // SNPRINTF_H