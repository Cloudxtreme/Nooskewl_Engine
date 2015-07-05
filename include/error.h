#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

class Error {
public:
	std::string error_message;
	Error() {}
	Error(std::string error_message) : error_message(error_message) {}
};

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

#endif // EXCEPTIONS_H