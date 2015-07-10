#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/util.h"

using namespace Nooskewl_Engine;

namespace Nooskewl_Engine {

Module m;

void load_dll()
{
	List_Directory ld(".\\*.dll");
	std::string filename;

	while ((filename = ld.next()) != "") {
		HINSTANCE hGetProcIDDLL = LoadLibrary(filename.c_str());

		if (!hGetProcIDDLL) {
			throw FileNotFoundError("Couldn't load game.dll!");
		}

		m.get_map_logic = (Map_Logic_Getter)GetProcAddress(hGetProcIDDLL, "get_map_logic");
		if (m.get_map_logic != NULL) {
			return;
		}
	}

	throw FileNotFoundError("Couldn't find a game DLL!");
}

#ifdef _MSC_VER

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

} // End namespace Nooskewl_Engine