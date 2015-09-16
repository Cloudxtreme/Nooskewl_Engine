#ifndef SAMPLE_H
#define SAMPLE_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Sample {
public:
	Sample(std::string filename);
	~Sample();

	static void update();

	Uint32 get_length();

	bool play(float volume, bool loop);
	bool play(float volume, Uint32 silence, Uint32 play_length);

	void stop_all();

private:
	SDL_AudioSpec *spec;
	Uint8 *data;
	Uint32 length;
};

struct SampleInstance {
	SDL_AudioSpec *spec;
	Uint8 *data;
	Uint32 length;
	Uint32 play_length;
	Uint32 offset;
	Uint32 silence;
	bool loop;
	float volume;
};

} // End namespace Nooskewl_Engine

#endif // SAMPLE_H
