#include "Nooskewl_Engine/tokenizer.h"

using namespace Nooskewl_Engine;

Tokenizer::Tokenizer(std::string s, char delimiter) :
	s(s),
	delimiter(delimiter),
	offset(0)
{
}

std::string Tokenizer::next()
{
	if (offset == s.length()) {
		return "";
	}

	size_t next_delim = s.find(delimiter, offset);
	std::string ret;

	if (next_delim == std::string::npos) {
		ret = s.substr(offset);
		offset = s.length();
		return ret;
	}

	ret = s.substr(offset, next_delim-offset);
	offset = next_delim + 1;

	return ret;
}