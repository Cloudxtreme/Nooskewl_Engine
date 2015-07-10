#ifndef MML_H
#define MML_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"

namespace Nooskewl_Engine {

	class Real_MML {
	public:
		Real_MML(std::string filename) throw (Error);
		~Real_MML();

		void mix(Uint8 *buf, int stream_length);

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

			void play(bool loop);
			void stop();
			// Must be called regularly (~1.0/60.0 seconds). Returns true if generated any samples.
			bool update(short *buf, int length);

			bool is_playing();

		private:
			void reset();

			void pulse(short *buf, size_t samples, float t, float frequency, float phase);
			void noise(short *buf, size_t samples, float t, float frequency, float phase);
			void sawtooth(short *buf, size_t samples, float t, float frequency, float phase);
			void sine(short *buf, size_t samples, float t, float frequency, float phase);
			void triangle(short *buf, size_t samples, float t, float frequency, float phase);
			void generate(short *buf, int samples, float t, const char *tok, int octave);

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

}

#endif // MML_H