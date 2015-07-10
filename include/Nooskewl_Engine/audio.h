#ifndef AUDIO_H
#define AUDIO_H

#include "Nooskewl_Engine/mml.h"

class EXPORT MML {
public:
	MML(std::string filename) throw (Error);
	~MML();

	void play(bool loop);
	void stop();
	void mix(Uint8 *buf, int stream_len);

private:
	Real_MML *real_mml;
};

void init_audio(int argc, char **argv) throw (Error);
void shutdown_audio();

#endif // AUDIO_H