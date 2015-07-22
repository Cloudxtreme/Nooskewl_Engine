#ifndef UTF8_H
#define UTF8_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

int utf8_len(std::string text);
Uint32 utf8_char_next(std::string text, int &offset);
Uint32 utf8_char(std::string text, int i);
std::string utf8_char_to_string(Uint32 ch);
std::string utf8_substr(std::string s, int start, int len = -1);

}

#endif