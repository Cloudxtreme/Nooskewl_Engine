#ifdef _MSC_VER

#include "Nooskewl_Engine/internal.h"

using namespace Nooskewl_Engine;

int c99_vsnprintf(char* str, int size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

int c99_snprintf(char* str, int size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#endif // _MSC_VER
