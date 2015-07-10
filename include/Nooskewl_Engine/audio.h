#ifndef AUDIO_H
#define AUDIO_H

#include "Nooskewl_Engine/mml.h"

namespace Nooskewl_Engine {

class NOOSKEWL_EXPORT MML {
public:
	MML(std::string filename) throw (Error);
	~MML();

	void play(bool loop);
	void stop();

private:
	MML_Internal *real_mml;
};

} // End namespace Nooskewl_Engine

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // AUDIO_H