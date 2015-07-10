#ifndef INTERNAL_H
#define INTERNAL_H

#include "Nooskewl_Engine/error.h"

#ifdef NOOSKEWL_ENGINE_BUILD

using namespace Nooskewl_Engine;

void init_audio(int argc, char **argv);
void shutdown_audio();

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

void load_fonts();
void release_fonts();
void init_font();
void shutdown_font();

void init_graphics();
void shutdown_graphics();
void update_graphics();
void load_palette(std::string name);

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
int c99_snprintf(char* str, int size, const char* format, ...);

void init_video(int argc, char **argv);
void shutdown_video();

void set_map_transition_projection(float angle);

void release_graphics();
void reload_graphics();

#endif

#endif INTERNAL_H