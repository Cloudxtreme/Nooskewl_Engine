#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <vector>

#include <SDL_audio.h>

typedef signed short Int16;

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

class Track
{
public:
	enum Type {
		PULSE = 0,
		NOISE,
		SAWTOOTH,
		SINE,
		TRIANGLE
	};

	// pad is # of samples of silence to pad the end with so all tracks in audio are even
	Track(Type type, std::string audio, std::vector< std::pair<int, float> > &volumes, std::vector<int> &pitches, std::vector< std::vector<float> > &pitch_envelopes, std::vector< std::pair<int, float> > &dutycycles, int pad);
	~Track();

	void play(bool looping);
	// Must be called regularly (~1.0/60.0 seconds). Returns true if generated any samples.
	bool update(Int16 *buf, int length);

private:
	void reset();

	void pulse(Int16 *buf, size_t samples, double t, float frequency, float phase);
	void noise(Int16 *buf, size_t samples, double t, float frequency, float phase);
	void sawtooth(Int16 *buf, size_t samples, double t, float frequency, float phase);
	void sine(Int16 *buf, size_t samples, double t, float frequency, float phase);
	void triangle(Int16 *buf, size_t samples, double t, float frequency, float phase);
	void generate(Int16 *buf, int samples, double t, const char *tok, int octave);

	float get_frequency(float start_freq);
	float get_volume();
	float get_dutycycle();

	std::string next_note(const char *audio, int *pos);
	int notelength(const char *tok, const char *audio, int *pos);

	Type type;
	std::string audio;
	std::vector< std::pair<int, float> > volumes;
	std::vector<int> pitches;
	std::vector< std::vector<float> > pitch_envelopes;
	std::vector< std::pair<int, float> > dutycycles;
	int pad;

	double dutycycle;
	int octave;
	int note_length;
	float volume;
	int tempo;
	int sample;
	int note;
	int volume_section;
	int dutycycle_section;
	double t;
	int pos;
	std::string tok;
	int length_in_samples;
	int buffer_fulfilled;
	int note_fulfilled;
	bool done;
	bool padded;
	bool looping;
};

typedef std::vector<Track *> *Audio;

void update_audio();

Audio load_audio(std::string filename);
void play_audio(Audio audio, bool looping);
void destroy_audio(Audio tracks);

bool init_audio();
void shutdown_audio();

#endif // AUDIO_H