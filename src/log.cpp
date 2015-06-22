#include <cstdarg>
#include <cstdio>

#include "log.h"

void logmsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
}