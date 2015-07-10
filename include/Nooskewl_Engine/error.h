#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Error {
public:
	std::string error_message;
	Error() {}
	Error(std::string error_message) : error_message(error_message) {}
};

class NOOSKEWL_ENGINE_EXPORT MemoryError : public Error {
public:
	MemoryError(std::string error_message) {
		this->error_message = "Memory error: " + error_message;
	}
};

class NOOSKEWL_ENGINE_EXPORT LoadError : public Error {
public:
	LoadError(std::string error_message) {
		this->error_message = "Load error: " + error_message;
	}
};

class NOOSKEWL_ENGINE_EXPORT FileNotFoundError : public Error {
public:
	FileNotFoundError(std::string error_message) {
		this->error_message = "File not found: " + error_message;
	}
};

class NOOSKEWL_ENGINE_EXPORT GLError : public Error {
public:
	GLError(std::string error_message) {
		this->error_message = "OpenGL error: " + error_message;
	}
};

} // End namespace Nooskewl_Engine

#endif // EXCEPTIONS_H