#ifndef LOG_H
#define LOG_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

NOOSKEWL_ENGINE_EXPORT void errormsg(const char *fmt, ...);
NOOSKEWL_ENGINE_EXPORT void infomsg(const char *fmt, ...);

} // End namespace Nooskewl_Engine

#endif // LOG_H