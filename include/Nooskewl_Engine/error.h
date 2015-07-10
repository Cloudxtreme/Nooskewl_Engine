#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

	class EXPORT Error {
	public:
		std::string error_message;
		Error() {}
		Error(std::string error_message) : error_message(error_message) {}
	};

}

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;

class MemoryError : public Error {
public:
	MemoryError(std::string error_message) {
		this->error_message = "Memory error: " + error_message;
	}
};

class LoadError : public Error {
public:
	LoadError(std::string error_message) {
		this->error_message = "Load error: " + error_message;
	}
};

class FileNotFoundError : public Error {
public:
	FileNotFoundError(std::string error_message) {
		this->error_message = "File not found: " + error_message;
	}
};

class GLError : public Error {
public:
	GLError(std::string error_message) {
		this->error_message = "OpenGL error: " + error_message;
	}
};

#endif

#endif // EXCEPTIONS_H