#ifndef TWEEN_H
#define TWEEN_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/dllist.h"

namespace Nooskewl_Engine {

	class Tween;
	typedef std::vector< DLList<Tween *> * > Tweens;

	class EXPORT Tween {
	public:
		bool started;
		Uint32 start_time;

		Tween();
		virtual ~Tween();

		virtual bool update(Tween *tween) = 0;

		static void update_tweens(Tweens tweens);
		static void destroy_tweens(Tweens tweens);
	};

}

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // TWEEN_H