#ifndef SOUND_H
#define SOUND_H

class NOOSKEWL_ENGINE_EXPORT Sound {
public:
	virtual void play(bool loop) = 0;
	virtual ~Sound() {}
};

#endif // SOUND_H
