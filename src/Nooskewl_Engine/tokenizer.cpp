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

	size_t next_delim;

	size_t tmp_o = offset;

	while (true) {
		next_delim = s.find(delimiter, tmp_o);
		if (next_delim != 0 && s[next_delim-1] == '\\') {
			tmp_o = next_delim + 1;
			continue;
		}
		break;
	}
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
