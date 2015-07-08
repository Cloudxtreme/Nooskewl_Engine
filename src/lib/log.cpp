#include "Nooskewl_Engine/log.h"

void errormsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	printf("ERROR: ");
	vprintf(fmt, v);
	va_end(v);
}

void infomsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
}
