#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/sample.h"

using namespace Nooskewl_Engine;

Sample::Sample(std::string filename)
{
	filename = "samples/" + filename;

	SDL_RWops *file = open_file(filename);

	spec = SDL_LoadWAV_RW(file, true, &m.device_spec, &data, &length);

	if (spec == NULL) {
		SDL_RWclose(file);
		throw LoadError("SDL_LoadWAV_RW failed");
	}
}

Sample::~Sample()
{
	std::vector<SampleInstance *>::iterator it;
	for (it = m.playing_samples.begin(); it != m.playing_samples.end();) {
		SampleInstance *s = *it;
		if (s->spec == spec) {
			it = m.playing_samples.erase(it);
		}
		else {
			it++;
		}
	}
	SDL_FreeWAV(data);
}

bool Sample::play(float volume, bool loop)
{
	if (noo.mute) {
		return true;
	}

	SampleInstance *s = new SampleInstance();
	if (s == NULL) {
		return false;
	}

	s->spec = spec;
	s->data = data;
	s->length = length;
	s->offset = 0;
	s->loop = loop;
	s->volume = volume;

	SDL_LockMutex(m.sample_mutex);
	m.playing_samples.push_back(s);
	SDL_UnlockMutex(m.sample_mutex);

	return true;
}