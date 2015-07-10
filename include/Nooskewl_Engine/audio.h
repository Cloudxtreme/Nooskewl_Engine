#ifndef AUDIO_H
#define AUDIO_H

#include "Nooskewl_Engine/mml.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT MML {
public:
	MML(std::string filename) throw (Error);
	~MML();

	void play(bool loop);
	void stop();

private:
	MML_Internal *internals;
};

} // End namespace Nooskewl_Engine

#endif // AUDIO_H