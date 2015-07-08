#ifndef SNPRINTF_H
#define SNPRINTF_H

#ifdef _MSC_VER

#include "Nooskewl_Engine/main.h"

#define snprintf c99_snprintf

EXPORT int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
EXPORT int c99_snprintf(char* str, int size, const char* format, ...);

#endif // _MSC_VER

#endif // SNPRINTF_H
