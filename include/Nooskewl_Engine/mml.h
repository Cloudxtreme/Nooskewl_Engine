#ifndef MML_H
#define MML_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Sample;

class NOOSKEWL_ENGINE_EXPORT MML {
public:
	static Sample *bass_drum;
	static Sample *hihat;

	static void start();
	static void end();

	static void mix(Uint8 *buf, int stream_length);

	MML(std::string filename, bool load_from_filesystem = false);
	~MML();

	void play(bool loop);
	void stop();
	std::string get_name(); // returns same thing passed to constructor
	bool is_done();

private:
	class Internal {
	public:
		Internal(std::string filename, bool load_from_filesystem);
		~Internal();

		class Track
		{
		public:
			enum Type {
				PULSE = 0,
				NOISE,
				SAWTOOTH,
				SINE,
				TRIANGLE,
				BASS_DRUM,
				HIHAT
			};

			// pad is # of samples of silence to pad the end with so all tracks are even
			Track(Type type, std::string text, std::vector< std::pair<int, float> > &volumes, std::vector<int> &pitches, std::vector< std::vector<float> > &pitch_envelopes, std::vector< std::pair<int, float> > &dutycycles, int pad);
			~Track();

			void play(bool loop);
			void stop();
			// Must be called regularly (~1.0/60.0 seconds). Returns true if generated any samples.
			bool update(short *buf, int length);

			bool is_playing();
			bool is_done();

		private:
			void reset();

			void pulse(short *buf, size_t samples, float t, float frequency, float phase);
			void noise(short *buf, size_t samples, float t, float frequency, float phase);
			void sawtooth(short *buf, size_t samples, float t, float frequency, float phase);
			void sine(short *buf, size_t samples, float t, float frequency, float phase);
			void triangle(short *buf, size_t samples, float t, float frequency, float phase);
			void generate(short *buf, int length_in_samples, int samples, float t, const char *tok, int octave);

			float get_frequency(float start_freq);
			float get_volume();
			float get_dutycycle();

			std::string next_note(const char *text, int *pos);
			int notelength(const char *tok, const char *text, int *pos);

			Type type;
			std::string text;
			std::vector< std::pair<int, float> > volumes;
			std::vector<int> pitches;
			std::vector< std::vector<float> > pitch_envelopes;
			std::vector< std::pair<int, float> > dutycycles;
			int pad;

			float dutycycle;
			int octave;
			int note_length;
			float volume;
			int tempo;
			int sample;
			int note;
			int volume_section;
			int dutycycle_section;
			float t;
			int pos;
			std::string tok;
			int length_in_samples;
			int buffer_fulfilled;
			int note_fulfilled;
			bool done;
			bool padded;
			bool loop;
			bool playing;
		};

		std::vector<Track *> tracks;
	};

	Internal *internal;

	static std::vector<Internal *> loaded_mml;

	std::string name;
};

} // End namespace Nooskewl_Engine

#endif // MML_H
