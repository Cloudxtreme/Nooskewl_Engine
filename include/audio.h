#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_audio.h>

class Sample {
public:
	Sample();
	~Sample();

	static void update();

	bool load_wav(SDL_RWops *file);

	bool play(float volume, bool loop);

private:
	SDL_AudioSpec *spec;
	Uint8 *data;
	Uint32 length;
};

bool init_audio();
void shutdown_audio();

#endif // AUDIO_H