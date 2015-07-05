#include "starsquatters.h"
#include "audio.h"
#include "util.h"

#define PI M_PI
#define TWOPI (2.0 * PI)
#define STREAM_FREQUENCY 44100
#define TO_INT16(f) ((Int16)(f*32767))

const double dt = 1.0 / STREAM_FREQUENCY;

const float note_pitches[12][11] = {
	{ 16.352, 32.703, 65.406, 130.81, 261.63, 523.25, 1046.5, 2093.0, 4186.0, 8372.0, 16744.0 },
	{ 17.324, 34.648, 69.296, 138.59, 277.18, 554.37, 1108.7, 2217.5, 4434.9, 8869.8, 17739.7 },
	{ 18.354, 36.708, 73.416, 146.83, 293.66, 587.33, 1174.7, 2349.3, 4698.6, 9397.3, 18794.5 },
	{ 19.445, 38.891, 77.782, 155.56, 311.13, 622.25, 1244.5, 2489.0, 4978.0, 9956.1, 19912.1 },
	{ 20.602, 41.203, 82.407, 164.81, 329.63, 659.26, 1318.5, 2637.0, 5274.0, 10548.1, 21096.2 },
	{ 21.827, 43.654, 87.307, 174.61, 349.23, 698.46, 1396.9, 2793.8, 5587.7, 11175.3, 22350.6 },
	{ 23.125, 46.249, 92.499, 185.00, 369.99, 739.99, 1480.0, 2960.0, 5919.9, 11839.8, 23679.6 },
	{ 24.500, 48.999, 97.999, 196.00, 392.00, 783.99, 1568.0, 3136.0, 6271.9, 12543.9, 25087.7 },
	{ 25.957, 51.913, 103.83, 207.65, 415.30, 830.61, 1661.2, 3322.4, 6644.9, 13289.8, 26579.5 },
	{ 27.500, 55.000, 110.00, 220.00, 440.00, 880.00, 1760.0, 3520.0, 7040.0, 14080.0, 28160.0 },
	{ 29.135, 58.270, 116.54, 233.08, 466.16, 932.33, 1864.7, 3729.3, 7458.6, 14917.2, 29834.5 },
	{ 30.868, 61.735, 123.47, 246.94, 493.88, 987.77, 1975.5, 3951.1, 7902.1, 15804.3, 31608.5 }
};

const char indexes[7] = {
	9, 11, 0, 2, 4, 5, 7 // a, b, c, d, e, f, g
};

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
static std::vector<Audio> loaded_audio;

// FIXME: use a mutex
static bool lock = false;

Sample::Sample(std::string filename)
{
	SDL_RWops *file = open_file(filename);

	spec = SDL_LoadWAV_RW(file, true, &device_spec, &data, &length);

	if (spec == NULL) {
		SDL_RWclose(file);
		throw LoadError("SDL_LoadWAV_RW failed");
	}
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

static std::string token(const char *audio, int *pos)
{
	char tok[100];
	int i = 0;
	
	while (*(audio+*pos) != 0 && isspace(*(audio+*pos))) (*pos)++; // skip whitespace

	// Read the token
	if (*(audio+*pos) != 0) {
		tok[i++] = *(audio+*pos);
		(*pos)++;
	}

	while (*(audio+*pos) != 0 && (isdigit(*(audio+*pos)) || *(audio+*pos) == '+' || *(audio+*pos) == '-' || *(audio+*pos) == '.' || (tok[0] == '@' && *(audio+*pos) >= 'A' && *(audio+*pos) <= 'Z'))) {
		tok[i++] = *(audio+*pos);
		(*pos)++;
	}

	while (*(audio+*pos) != 0 && isspace(*(audio+*pos))) (*pos)++; // skip more whitespace

	tok[i] = 0;
	return tok;
}

// In samples
static int onenotelength(const char *tok, int note_length, int tempo, int octave, int note, char prev_ch, std::vector<int> &pitches, std::vector< std::vector<float> > &pitch_envelopes)
{
	char ch;
	if (*tok == 'w') {
		ch = prev_ch;
	}
	else {
		ch = *tok;
	}
	int index = -1;
	if (ch >= 'a' && ch <= 'g') {
		index = indexes[ch - 'a'];
		tok++;
		if (*tok == '+') {
			index++;
			tok++;
		}
		else if (*tok == '-') {
			index--;
			tok++;
		}
	}
	else {
		tok++;
	}
	int length = atoi(tok);
	int total = (STREAM_FREQUENCY / (tempo / 4.0f / 60.0f)) / (length == 0 ? note_length : length);
	int dotlength = total / 2;
	while (*tok != 0) {
		if (*tok == '.') {
			total += dotlength;
			dotlength /= 2;
		}
		tok++;
	}
	if (index == -1) {
		return total;
	}
	// Finish off a full phase to avoid clicks and pops
	int pitch = pitches[note];
	float frequency;
	if (pitch == -1) {
		frequency = note_pitches[index][octave];
	}
	else {
		frequency = pitch_envelopes[pitch][pitch_envelopes[pitch].size()-1];
	}
	float samples_per_phase = STREAM_FREQUENCY / frequency;
	float mod = fmod(total, samples_per_phase);
	if (mod <= 1) {
		return total;
	}
	total += samples_per_phase - mod;
	return total;
}

Track::Track(Type type, std::string audio, std::vector< std::pair<int, float> > &volumes, std::vector<int> &pitches, std::vector< std::vector<float> > &pitch_envelopes, std::vector< std::pair<int, float> > &dutycycles, int pad) :
	type(type),
	audio(audio),
	volumes(volumes),
	pitches(pitches),
	pitch_envelopes(pitch_envelopes),
	dutycycles(dutycycles),
	pad(pad),
	looping(looping)
{
}

Track::~Track()
{
}

void Track::play(bool looping)
{
	this->looping = looping;

	reset();
}

bool Track::update(Int16 *buf, int length)
{
	if (done) {
		return false;
	}

	while (buffer_fulfilled < length) {
		int to_generate = length_in_samples - note_fulfilled;
		int left_in_buffer = length - buffer_fulfilled;
		int fulfilled = buffer_fulfilled;
		if (left_in_buffer <= to_generate) {
			to_generate = left_in_buffer;
			fulfilled = length;
		}
		else {
			fulfilled += to_generate;
		}
		generate(buf + buffer_fulfilled, to_generate, t, tok.c_str(), octave);
		buffer_fulfilled = fulfilled;
		bool get_next_note = false;
		if (note_fulfilled >= length_in_samples) {
			get_next_note = true;
		}
		if (get_next_note) {
			const char *audio_cstr = audio.c_str();

			t = 0;
			tok = next_note(audio_cstr, &pos);
			note++;
			if (tok[0] == 0) {
				note--;
				if (padded) {
					if (looping) {
						reset();
					}
				}
				else {
					padded = true;
					if (looping) {
						length_in_samples = pad;
						tok = "r";
					}
					else {
						memset(buf, device_spec.silence, sizeof(float) * length);
						done = true;
						return true;
					}
				}
			}
			else {
				length_in_samples = notelength(tok.c_str(), audio_cstr, &pos);
			}
			note_fulfilled = 0;
		}
		else {
			t += dt * to_generate;
		}
	}
	buffer_fulfilled = 0;

	return true;
}

void Track::reset()
{
	dutycycle = 0.5f;
	octave = 4;
	note_length = 4;
	volume = 1.0f;
	tempo = 120;
	sample = 0;
	note = 0;
	volume_section = 0;
	dutycycle_section = 0;
	t = 0.0;
	pos = 0;
	tok = next_note(audio.c_str(), &pos);
	length_in_samples = notelength(tok.c_str(), audio.c_str(), &pos);
	buffer_fulfilled = 0;
	note_fulfilled = 0;
	padded = false;
	done = false;
}

void Track::pulse(Int16 *buf, size_t samples, double t, float frequency, float phase)
{
	unsigned i;

	for (i = 0; i < samples; i++) {
		double w = TWOPI * get_frequency(frequency);
		double ti = t + i * dt;
		double a = fmod(w * ti + phase, TWOPI);
		double x;

		if (a < TWOPI*get_dutycycle()) {
			x = 1;
		}
		else {
			x = -1;
		}

		buf[i] = TO_INT16(x * get_volume());

		sample++;
		note_fulfilled++;
	}
}

void Track::noise(Int16 *buf, size_t samples, double t, float frequency, float phase)
{
	unsigned i;

	for (i = 0; i < samples; i++) {
		double w = TWOPI * get_frequency(frequency);
		double ti = t + i * dt;
		double a = fmod(w * ti + phase, TWOPI);

		float r = (rand() % 255 / 255.0); // FIXME: use different rand

		float v = fmod(a, TWOPI) / TWOPI * get_volume();

		buf[i] = TO_INT16(r * v);

		sample++;
		note_fulfilled++;
	}
}

void Track::sawtooth(Int16 *buf, size_t samples, double t, float frequency, float phase)
{
	unsigned i;

	for (i = 0; i < samples; i++) {
		double w = TWOPI * get_frequency(frequency);
		double tx = w * (t + i * dt) + PI + phase;
		double tu = fmod(tx/PI, 2.0);

		buf[i] = TO_INT16((-1.0 + tu) * get_volume());

		sample++;
		note_fulfilled++;
	}
}

void Track::sine(Int16 *buf, size_t samples, double t, float frequency, float phase)
{
	unsigned i;

	for (i = 0; i < samples; i++) {
		double w = TWOPI * get_frequency(frequency);
		double ti = t + i * dt;
		buf[i] = TO_INT16(sin(w * ti + phase) * get_volume());

		sample++;
		note_fulfilled++;
	}
}

void Track::triangle(Int16 *buf, size_t samples, double t, float frequency, float phase)
{
	unsigned i;

	for (i = 0; i < samples; i++) {
		double w = TWOPI * get_frequency(frequency);
		double tx = w * (t + i * dt) + PI/2.0 + phase;
		double tu = fmod(tx/PI, 2.0);

		if (tu <= 1.0)
			buf[i] = TO_INT16((1.0 - 2.0 * tu) * get_volume());
		else
			buf[i] = TO_INT16((-1.0 + 2.0 * (tu - 1.0)) * get_volume());

		sample++;
		note_fulfilled++;
	}
}

void Track::generate(Int16 *buf, int samples, double t, const char *tok, int octave)
{
	char c = tok[0];
	int index = 0;
	if (c == 'r') {
		memset(buf, device_spec.silence, sizeof(Int16)*samples);
		sample += samples;
		note_fulfilled += samples;
		return;
	}
	else if (c >= 'a' && c <= 'g') {
		index = indexes[c-'a'];
		if (tok[1] == '+') index++;
		else if (tok[1] == '-') index--;
	}
	float pitch = note_pitches[index][octave];

	switch (type) {
		case PULSE:
			pulse(buf, samples, t, pitch, 0.0f);
			break;
		case NOISE:
			noise(buf, samples, t, pitch, 0.0f);
			break;
		case SAWTOOTH:
			sawtooth(buf, samples, t, pitch, 0.0f);
			break;
		case SINE:
			sine(buf, samples, t, pitch, 0.0f);
			break;
		case TRIANGLE:
			triangle(buf, samples, t, pitch, 0.0f);
			break;
	};
}

float Track::get_frequency(float start_freq)
{
	int pitch = pitches[note];
	if (pitch == -1) {
		return start_freq;
	}
	float p = note_fulfilled / (float)length_in_samples;
	int i = p * pitch_envelopes[pitch].size();
	if (i > 0) {
		start_freq = pitch_envelopes[pitch][i-1];
	}
	float stride = 1.0f / pitch_envelopes[pitch].size();
	float start = i * stride;
	float p2 = (p - start) / stride;
	return (pitch_envelopes[pitch][i] * p2) + (start_freq * (1.0f - p2));
}

float Track::get_volume()
{
	while (volume_section < (int)volumes.size()-1 && sample > volumes[volume_section+1].first) {
		volume_section++;
	}
	if (volume_section >= (int)volumes.size()) {
		return volumes[volumes.size()-1].second;
	}
	else {
		float p = (sample - volumes[volume_section].first) / (float)(volumes[volume_section+1].first - volumes[volume_section].first);
		return ((1-p) * volumes[volume_section].second) + (p * volumes[volume_section+1].second);
	}
}

float Track::get_dutycycle()
{
	while (dutycycle_section < (int)dutycycles.size()-1 && sample > dutycycles[dutycycle_section+1].first) {
		dutycycle_section++;
	}
	if (dutycycle_section >= (int)dutycycles.size()) {
		return dutycycles[dutycycles.size()-1].second;
	}
	else {
		float p = (sample - dutycycles[dutycycle_section].first) / (float)(dutycycles[dutycycle_section+1].first - dutycycles[dutycycle_section].first);
		return ((1-p) * dutycycles[dutycycle_section].second) + (p * dutycycles[dutycycle_section+1].second);
	}
}

std::string Track::next_note(const char *audio, int *pos)
{
	std::string result;
	bool done = false;
	do {
		result = token(audio, pos);
		switch (result.c_str()[0]) {
			case '<':
			case '>':
				if (result.c_str()[0] == '<') {
					octave--;
				}
				else {
					octave++;
				}
				if (octave < 0) {
					octave = 0;
				}
				else if (octave > 11) {
					octave = 11;
				}
				break;
			case 'o':
				octave = atoi(result.c_str()+1);
				break;
			case 'w':
				break;
			case 'l':
				note_length = atoi(result.c_str()+1);
				break;
			case 't':
				tempo = atoi(result.c_str() + 1);
				break;
			case '@':
				if (!strncmp(result.c_str()+1, "TYPE", 4)) {
					type = (Type)atoi(result.c_str() + 5);
				}
				break;
			case 'v':
				// doesn't need to do this -- volumes set with get_volume
				break;
			case 'y':
				// doesn't need to do this -- dutycycles set with get_dutycycle
				break;
			default:
				done = true;
				break;
		}
	} while (!done);
	return result;
}

// adds waits to length
int Track::notelength(const char *tok, const char *audio, int *pos)
{
	char ch = *tok;
	int total = onenotelength(tok, note_length, tempo, octave, note, 'z', pitches, pitch_envelopes); // z == nothing never used
	int pos2 = *pos;
	std::string tok2;
	do {
		tok2 = token(audio, &pos2);
		if (tok2.c_str()[0] == 'w') {
			total += onenotelength(tok2.c_str(), note_length, tempo, octave, note, ch, pitches, pitch_envelopes);
		}
	} while (tok2.c_str()[0] == 'w');
	return total;
}

void update_audio(Uint8 *buf, int stream_length)
{
	Uint8 *tmp = new Uint8[stream_length];

	for (size_t audio = 0; audio < loaded_audio.size(); audio++) {
		Audio tracks = loaded_audio[audio];
		for (size_t track = 0; track < tracks->size(); track++) {
			bool filled = (*tracks)[track]->update((Int16 *)tmp, stream_length/2);
			if (filled) {
				SDL_MixAudioFormat(buf, tmp, device_spec.format, stream_length, 16);
			}
		}
	}

	delete[] tmp;
}

Audio load_audio(std::string filename)
{
	std::vector<std::string> tracks_s;
	std::vector< std::vector< std::pair<int, float> > > volumes;
	std::vector< std::vector< std::pair<int, float> > > dutycycles;
	std::vector< std::vector<int> > pitches;
	std::vector<int> note_lengths;
	std::vector<int> tempos;
	std::vector<int> octaves;
	std::vector<int> sample; // samples read
	std::vector<int> vol_start; // volume envelope start sample
	std::vector<int> curr_vol; // current volume envelope
	std::vector<int> duty_start; // dutycycle envelope start sample
	std::vector<int> curr_duty; // current dutycycle envelope
	std::vector<int> note;
	std::vector<int> current_pitch;
	std::vector<char> prev_ch;
	std::vector< std::vector<int> > volume_envelopes;
	std::vector< std::vector<float> > pitch_envelopes;
	std::vector< std::vector<int> > dutycycle_envelopes;
	char buf[1000];

	SDL_RWops *f = open_file(filename);

	while (SDL_fgets(f, buf, 1000)) {
		int pos = 0;
		std::string tok = token(buf, &pos);
		if (tok.c_str()[0] >= 'A' && tok.c_str()[0] <= 'Z') {
			int track = tok.c_str()[0] - 'A';
			while ((int)tracks_s.size() < track+1) {
				tracks_s.push_back("");
				volumes.push_back(std::vector< std::pair<int, float> >());
				volumes[volumes.size()-1].push_back(std::pair<int, float>(0, 1.0f));
				dutycycles.push_back(std::vector< std::pair<int, float> >());
				dutycycles[dutycycles.size()-1].push_back(std::pair<int, float>(0, 0.5f));
				pitches.push_back(std::vector<int>());
				note_lengths.push_back(4);
				tempos.push_back(120);
				octaves.push_back(4);
				sample.push_back(0);
				vol_start.push_back(0);
				curr_vol.push_back(-1);
				duty_start.push_back(0);
				curr_duty.push_back(-1);
				note.push_back(0);
				current_pitch.push_back(-1);
				prev_ch.push_back('z');
			}
			while (isspace(buf[strlen(buf) - 1]))
				buf[strlen(buf) - 1] = 0;
			tracks_s[track] += std::string(buf + pos);
			/* Pre-process for envelopes */
			tok = token(buf, &pos);
			while (tok.c_str()[0]) {
				if (tok.c_str()[0] == '@') {
					if (tok.c_str()[1] == 'V') {
						if (curr_vol[track] == -1) {
							vol_start[track] = sample[track];
							curr_vol[track] = atoi(tok.c_str() + 2);
						}
						else {
							if (volumes[track].size() > 0) {
								volumes[track].push_back(std::pair<int, float>(
									sample[track],
									volumes[track][volumes[track].size()-1].second
								));
							}
							int length = sample[track] - vol_start[track];
							if ((int)volume_envelopes.size() > curr_vol[track]) {
								int sz = volume_envelopes[curr_vol[track]].size();
								int stride = length / (sz-1);
								for (int i = 0; i < sz; i++) {
									volumes[track].push_back(std::pair<int, float>(
										vol_start[track] + stride * i,
										volume_envelopes[curr_vol[track]][i]/255.0f
									));
								}
							}
							curr_vol[track] = -1;
						}
					}
					else if (tok.c_str()[1] == 'P') {
						current_pitch[track] = atoi(tok.c_str() + 2);
						if (current_pitch[track] >= (int)pitch_envelopes.size()) {
							current_pitch[track] = -1;
						}
					}
					else if (tok.c_str()[1] == 'D') {
						if (curr_duty[track] == -1) {
							duty_start[track] = sample[track];
							curr_duty[track] = atoi(tok.c_str() + 2);
						}
						else {
							if (dutycycles[track].size() > 0) {
								dutycycles[track].push_back(std::pair<int, float>(
									sample[track],
									dutycycles[track][dutycycles[track].size()-1].second
								));
							}
							int length = sample[track] - duty_start[track];
							if ((int)dutycycle_envelopes.size() > curr_duty[track]) {
								int sz = dutycycle_envelopes[curr_duty[track]].size();
								int stride = length / (sz-1);
								for (int i = 0; i < sz; i++) {
									dutycycles[track].push_back(std::pair<int, float>(
										duty_start[track] + stride * i,
										dutycycle_envelopes[curr_duty[track]][i]/255.0f
									));
								}
							}
							curr_duty[track] = -1;
						}
					}
				}
				else {
					if ((tok.c_str()[0] >= 'a' && tok.c_str()[0] <= 'g') || tok.c_str()[0] == 'r' || tok.c_str()[0] == 'w') {
						if (tok.c_str()[0] >= 'a' && tok.c_str()[0] <= 'g') {
							prev_ch[track] = tok.c_str()[0];
						}
						pitches[track].push_back(current_pitch[track]);
						sample[track] += onenotelength(tok.c_str(), note_lengths[track], tempos[track], octaves[track], note[track], prev_ch[track], pitches[track], pitch_envelopes);
						current_pitch[track] = -1;
						note[track]++;
					}
					else if (tok.c_str()[0] == 'v') {
						float vol = atoi(tok.c_str() + 1) / 255.0f;
						if (volumes[track].size() > 0) {
							volumes[track].push_back(std::pair<int, float>(
								sample[track],
								volumes[track][volumes[track].size()-1].second
							));
						}
						volumes[track].push_back(std::pair<int, float>(sample[track], vol));
					}
					else if (tok.c_str()[0] == 'y') {
						float duty = atoi(tok.c_str() + 1) / 255.0f;
						if (dutycycles[track].size() > 0) {
							dutycycles[track].push_back(std::pair<int, float>(
								sample[track],
								dutycycles[track][dutycycles[track].size()-1].second
							));
						}
						dutycycles[track].push_back(std::pair<int, float>(sample[track], duty));
					}
					else if (tok.c_str()[0] == 'l') {
						note_lengths[track] = atoi(tok.c_str() + 1);
					}
					else if (tok.c_str()[0] == 't') {
						tempos[track] = atoi(tok.c_str() + 1);
					}
					else if (tok.c_str()[0] == 'o') {
						octaves[track] = atoi(tok.c_str() + 1);
					}
					else if (tok.c_str()[0] == '<') {
						octaves[track]--;
						if (octaves[track] < 0) {
							octaves[track] = 0;
						}
					}
					else if (tok.c_str()[0] == '>') {
						octaves[track]++;
						if (octaves[track] > 11) {
							octaves[track] = 11;
						}
					}
				}
				tok = token(buf, &pos);
			}
		}
		else if (tok.c_str()[0] == '@') {
			if (tok.c_str()[1] == 'V') {
				int num = atoi(tok.c_str() + 2);
				std::vector<int> envelope;
				while (tok.c_str()[0] != 0) {
					tok = token(buf, &pos);
					if (isdigit(tok.c_str()[0])) {
						envelope.push_back(atoi(tok.c_str()));
					}
				}
				while ((int)volume_envelopes.size() <= num) {
					volume_envelopes.push_back(std::vector<int>());
				}
				volume_envelopes[num] = envelope;
			}
			else if (tok.c_str()[1] == 'P') {
				int num = atoi(tok.c_str() + 2);
				std::vector<float> envelope;
				while (tok.c_str()[0] != 0) {
					tok = token(buf, &pos);
					if (tok.c_str()[0] >= 'a' && tok.c_str()[0] <= 'g') {
						int index = indexes[tok.c_str()[0] - 'a'];
						int offset = 1;
						if (tok.c_str()[1] == '+') {
							index++;
							offset++;
						}
						else if (tok.c_str()[1] == '-') {
							index--;
							offset++;
						}
						int octave = atoi(tok.c_str() + offset);
						if (octave < 0) {
							octave = 0;
						}
						else if (octave > 11) {
							octave = 11;
						}
						float pitch = note_pitches[index][octave];
						envelope.push_back(pitch);
					}
				}
				while ((int)pitch_envelopes.size() <= num) {
					pitch_envelopes.push_back(std::vector<float>());
				}
				pitch_envelopes[num] = envelope;
			}
			else if (tok.c_str()[1] == 'D') {
				int num = atoi(tok.c_str() + 2);
				std::vector<int> envelope;
				while (tok.c_str()[0] != 0) {
					tok = token(buf, &pos);
					if (isdigit(tok.c_str()[0])) {
						envelope.push_back(atoi(tok.c_str()));
					}
				}
				while ((int)dutycycle_envelopes.size() <= num) {
					dutycycle_envelopes.push_back(std::vector<int>());
				}
				dutycycle_envelopes[num] = envelope;
			}
		}
	}
	SDL_RWclose(f);

	int longest = 0;

	for (size_t i = 0; i < tracks_s.size(); i++) {
		if (sample[i] > longest) {
			longest = sample[i];
		}

		// Volumes and duty cycles must have an explicit ending

		volumes[i].push_back(std::pair<int, float>(
			sample[i],
			volumes[i][volumes[i].size() - 1].second
			));

		dutycycles[i].push_back(std::pair<int, float>(
			sample[i],
			dutycycles[i][dutycycles[i].size() - 1].second
			));
	}

	std::vector<Track *> *tracks = new std::vector<Track *>();

	for (size_t i = 0; i < tracks_s.size(); i++) {
		tracks->push_back(new Track(Track::PULSE, tracks_s[i], volumes[i], pitches[i], pitch_envelopes, dutycycles[i], longest-sample[i]));
	}

	loaded_audio.push_back(tracks);

	return tracks;
}

void play_audio(Audio tracks, bool looping)
{
	for (size_t i = 0; i < tracks->size(); i++) {
		(*tracks)[i]->play(looping);
	}
}

void destroy_audio(Audio tracks)
{
	std::vector<Audio>::iterator it = std::find(loaded_audio.begin(), loaded_audio.end(), tracks);

	if (it != loaded_audio.end()) {
		loaded_audio.erase(it);
	}

	for (size_t i =  0; i < tracks->size(); i++) {
		delete (*tracks)[i];
	}

	delete tracks;
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
	update_audio(stream, stream_length);
	lock = false;
}

void init_audio()
{
	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = AUDIO_S16;
	desired.channels = 1;
	desired.samples = 4096;
	desired.callback = audio_callback;
	desired.userdata = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, false, &desired, &device_spec, 0);

	if (audio_device == 0) {
		throw Error("init_audio failed");
	}

	SDL_PauseAudioDevice(audio_device, false);
}

void shutdown_audio()
{
	SDL_CloseAudioDevice(audio_device);
}