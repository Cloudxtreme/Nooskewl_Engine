#include "Nooskewl_Engine/audio.h"
#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/module.h"
#include "Nooskewl_Engine/sample.h"

using namespace Nooskewl_Engine;

static SDL_AudioDeviceID audio_device;
static std::vector<MML_Internal *> loaded_mml;

static void update_mml(Uint8 *buf, int stream_length)
{
	for (size_t i = 0; i < loaded_mml.size(); i++) {
		loaded_mml[i]->mix(buf, stream_length);
	}
}

static void audio_callback(void *userdata, Uint8 *stream, int stream_length)
{
	memset(stream, m.device_spec.silence, stream_length);

	SDL_LockMutex(m.sample_mutex);
	std::vector<SampleInstance *>::iterator it;
	for (it = m.playing_samples.begin(); it != m.playing_samples.end();) {
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
		SDL_MixAudioFormat(stream, s->data+s->offset, m.device_spec.format, length, (int)(s->volume * 128.0f));
		if (s->loop == false && s->offset >= s->length) {
			it = m.playing_samples.erase(it);
		}
		else {
			it++;
		}
	}
	SDL_UnlockMutex(m.sample_mutex);

	update_mml(stream, stream_length);
}

void init_audio(int argc, char **argv)
{
	if (g.audio.mute) {
		return;
	}

	m.sample_mutex = SDL_CreateMutex();

	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = AUDIO_S16;
	desired.channels = 1;
	desired.samples = 4096;
	desired.callback = audio_callback;
	desired.userdata = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, false, &desired, &m.device_spec, 0);

	if (audio_device == 0) {
		throw Error("init_audio failed");
	}

	SDL_PauseAudioDevice(audio_device, false);
}

void shutdown_audio()
{
	if (audio_device != 0) {
		SDL_CloseAudioDevice(audio_device);
	}

	SDL_DestroyMutex(m.sample_mutex);
}

MML::MML(std::string filename)
{
	internals = new MML_Internal(filename);
	loaded_mml.push_back(internals);
}

MML::~MML()
{
	for (size_t i = 0; i < loaded_mml.size(); i++) {
		if (loaded_mml[i] == internals) {
			loaded_mml.erase(loaded_mml.begin()+i);
			break;
		}
	}
	delete internals;
}

void MML::play(bool loop)
{
	if (g.audio.mute) {
		return;
	}

	std::vector<MML_Internal::Track *> &tracks = internals->tracks;

	for (size_t i = 0; i < tracks.size(); i++) {
		tracks[i]->play(loop);
	}
}

void MML::stop()
{
	std::vector<MML_Internal::Track *> &tracks = internals->tracks;

	for (size_t i = 0; i < tracks.size(); i++) {
		tracks[i]->stop();
	}
}