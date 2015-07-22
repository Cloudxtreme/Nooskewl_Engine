#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Translation {
public:
	Translation(std::string text);

	std::string translate(int id);

private:
	std::map<int, std::string> translation;
};

} // End namespace Nooskewl_Engine

#endif // TRANSLATION_H