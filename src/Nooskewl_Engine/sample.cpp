#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/sample.h"

using namespace Nooskewl_Engine;

Sample::Sample(std::string filename)
{
	filename = "samples/" + filename;

	SDL_RWops *file = open_file(filename);

	spec = SDL_LoadWAV_RW(file, true, &m.device_spec, &data, &length);

	if (spec == 0) {
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

Uint32 Sample::get_length()
{
	return length / 2; // length is in bytes, function returns samples
}

void Sample::play(bool loop)
{
	play(1.0f, loop);
}

bool Sample::play(float volume, bool loop)
{
	if (noo.mute) {
		return true;
	}

	SampleInstance *s = new SampleInstance();
	if (s == 0) {
		return false;
	}

	s->spec = spec;
	s->data = data;
	s->length = length;
	s->play_length = length;
	s->offset = 0;
	s->silence = 0;
	s->loop = loop;
	s->volume = volume;

	SDL_LockMutex(m.mixer_mutex);
	m.playing_samples.push_back(s);
	SDL_UnlockMutex(m.mixer_mutex);

	return true;
}

// Play length is passed in in samples
bool Sample::play(float volume, Uint32 silence, Uint32 play_length)
{
	if (noo.mute) {
		return true;
	}

	SampleInstance *s = new SampleInstance();
	if (s == 0) {
		return false;
	}

	s->spec = spec;
	s->data = data;
	s->length = length;
	s->play_length = play_length * 2; // convert to bytes
	s->offset = 0;
	s->silence = silence * 2; // convert to bytes
	s->loop = false;
	s->volume = volume;

	SDL_LockMutex(m.mixer_mutex);
	m.playing_samples.push_back(s);
	SDL_UnlockMutex(m.mixer_mutex);

	return true;
}

void Sample::stop_all()
{
	SDL_LockMutex(m.mixer_mutex);

	std::vector<SampleInstance *>::iterator it;

	for (it = m.playing_samples.begin(); it != m.playing_samples.end();) {
		SampleInstance *s = *it;
		if (s->data == data) {
			it = m.playing_samples.erase(it);
		}
		else {
			it++;
		}
	}

	SDL_UnlockMutex(m.mixer_mutex);
}
