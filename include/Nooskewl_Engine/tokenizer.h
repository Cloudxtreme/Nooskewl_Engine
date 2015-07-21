#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Tokenizer {
public:

	Tokenizer::Tokenizer(std::string s, char delimiter);
	std::string next();

private:
	std::string s;
	char delimiter;
	size_t offset;
};

} // End namespace Nooskewl_Engine

#endif // TOKENIZER_H