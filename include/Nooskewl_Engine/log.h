#ifndef LOG_H
#define LOG_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

NOOSKEWL_EXPORT void errormsg(const char *fmt, ...);
NOOSKEWL_EXPORT void infomsg(const char *fmt, ...);

} // End namespace Nooskewl_Engine

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // LOG_H