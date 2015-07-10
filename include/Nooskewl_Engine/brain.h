#ifndef BRAIN_H
#define BRAIN_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

	class EXPORT Brain {
	public:
		bool l, r, u, d, b1;

		Brain();
		virtual ~Brain();

		virtual void handle_event(TGUI_Event *event) = 0;
	};
}

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // BRAIN_H