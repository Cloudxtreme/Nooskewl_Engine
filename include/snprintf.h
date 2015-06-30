#ifndef SNPRINTF_H
#define SNPRINTF_H

#ifdef _MSC_VER

#include <cstdarg>

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
int c99_snprintf(char* str, int size, const char* format, ...);

#endif // _MSC_VER

#endif // SNPRINTF_H