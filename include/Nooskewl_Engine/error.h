#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_EXPORT Error {
public:
	std::string error_message;
	Error() {}
	Error(std::string error_message) : error_message(error_message) {}
};

} // End namespace Nooskewl_Engine

#endif // EXCEPTIONS_H