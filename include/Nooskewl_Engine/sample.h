#ifndef SAMPLE_H
#define SAMPLE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Sample {
public:
	Sample(std::string filename) throw (Error);
	~Sample();

	static void update();

	bool play(float volume, bool loop);

private:
	SDL_AudioSpec *spec;
	Uint8 *data;
	Uint32 length;
};

struct SampleInstance {
	SDL_AudioSpec *spec;
	Uint8 *data;
	Uint32 length;
	Uint32 offset;
	bool loop;
	float volume;
};

} // End namespace Nooskewl_Engine

#endif // SAMPLE_H