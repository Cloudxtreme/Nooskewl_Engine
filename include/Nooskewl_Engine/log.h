#ifndef LOG_H
#define LOG_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

	EXPORT void errormsg(const char *fmt, ...);
	EXPORT void infomsg(const char *fmt, ...);

}

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // LOG_H