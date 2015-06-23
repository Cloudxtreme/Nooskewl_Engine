#include <vector>

#include "starsquatters.h"

struct SampleInstance {
	SDL_AudioSpec *spec;
	Uint8 *data;
	Uint32 length;
	Uint32 offset;
	bool loop;
	float volume;
};

static SDL_AudioDeviceID audio_device;
static SDL_AudioSpec device_spec;

static std::vector<SampleInstance *> playing_samples;

// FIXME: use a mutex
static bool lock = false;

Sample::Sample() :
	spec(NULL),
	data(NULL)
{
}

Sample::~Sample()
{
	std::vector<SampleInstance *>::iterator it;
	for (it = playing_samples.begin(); it != playing_samples.end();) {
		SampleInstance *s = *it;
		if (s->spec == spec) {
			it = playing_samples.erase(it);
		}
		else {
			it++;
		}
	}
	SDL_FreeWAV(data);
}

bool Sample::load_wav(SDL_RWops *file)
{
	spec = SDL_LoadWAV_RW(file, true, &device_spec, &data, &length);

	if (spec == NULL) {
		return false;
	}

	return true;
}

bool Sample::play(float volume, bool loop)
{
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

	while (lock);
	lock = true;
	playing_samples.push_back(s);
	lock = false;

	return true;
}

static void audio_callback(void *userdata, Uint8 *stream, int stream_length)
{
	memset(stream, device_spec.silence, stream_length);

	while (lock);
	lock = true;
	std::vector<SampleInstance *>::iterator it;
	for (it = playing_samples.begin(); it != playing_samples.end();) {
		SampleInstance *s = *it;
		int length;
		if (s->loop) {
			length = stream_length;
		}
		else {
			length = s->length - s->offset;
			if (length > stream_length) {
				length = stream_length;
			}
			s->offset += length;
		}
		SDL_MixAudioFormat(stream, s->data+s->offset, device_spec.format, length, (int)(s->volume * 128.0f));
		if (s->loop == false && s->offset >= s->length) {
			it = playing_samples.erase(it);
		}
		else {
			it++;
		}
	}
	lock = false;
}

bool init_audio()
{
	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = AUDIO_S16;
	desired.channels = 2;
	desired.samples = 4096;
	desired.callback = audio_callback;
	desired.userdata = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, false, &desired, &device_spec, 0);

	if (audio_device == 0) {
		return false;
	}

	SDL_PauseAudioDevice(audio_device, false);

	return true;
}

void shutdown_audio()
{
	SDL_CloseAudioDevice(audio_device);
}