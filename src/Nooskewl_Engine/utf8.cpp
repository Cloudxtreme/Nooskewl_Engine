#include "Nooskewl_Engine/utf8.h"

namespace Nooskewl_Engine {

static int utf8_size(unsigned const char *p)
{
	unsigned char ch = *p;

	if (ch == 0) {
		return 0;
	}

	if ((ch & 0x80) == 0) {
		return 1;
	}
	else if ((ch & 0xE0) == 0xC0) {
		return 2;
	}
	else if ((ch & 0xF0) == 0xE0) {
		return 3;
	}
	else if ((ch & 0xF8) == 0xF0) {
		return 4;
	}
	else if ((ch & 0xFC) == 0xF8) {
		return 5;
	}
	else {
		return 6;
	}
}

static Uint32 utf8_next(unsigned const char *p)
{
	unsigned char ch = *p;
	Uint32 result;

	if ((ch & 0x80) == 0) {
		result = *p;
	}
	else if ((ch & 0xE0) == 0xC0) {
		unsigned char a = *p++;
		unsigned char b = *p;
		result = ((a & 0x1F) << 6) | (b & 0x3F);
	}
	else if ((ch & 0xF0) == 0xE0) {
		unsigned char a = *p++;
		unsigned char b = *p++;
		unsigned char c = *p;
		result = ((a & 0xF) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
	}
	else if ((ch & 0xF8) == 0xF0) {
		unsigned char a = *p++;
		unsigned char b = *p++;
		unsigned char c = *p++;
		unsigned char d = *p;
		result = ((a & 0x7) << 18) | ((b & 0x3F) << 12) | ((c & 0x3F) << 6) | (d & 0x3F);
	}
	else if ((ch & 0xFC) == 0xF8) {
		unsigned char a = *p++;
		unsigned char b = *p++;
		unsigned char c = *p++;
		unsigned char d = *p++;
		unsigned char e = *p;
		result = ((a & 0x3) << 24) | ((b & 0x3F) << 18) | ((c & 0x3F) << 12) | ((d & 0x3F) << 6) | (e & 0x3F);
	}
	else if ((ch & 0xFE) == 0xFC) {
		unsigned char a = *p++;
		unsigned char b = *p++;
		unsigned char c = *p++;
		unsigned char d = *p++;
		unsigned char e = *p++;
		unsigned char f = *p;
		result = ((a & 0x1) << 30) | ((b & 0x3F) << 24) | ((c & 0x3F) << 18) | ((d & 0x3F) << 12) | ((e & 0x3F) << 6) | (e & 0x3F);
	}

	return result;
}

int utf8_len(std::string text)
{
	unsigned const char *p = (unsigned const char *)text.c_str();
	int len = 0;

	while (*p != 0) {
		len++;
		p += utf8_size(p);
	}

	return len;
}

Uint32 utf8_char_next(std::string text, int &offset)
{
	unsigned const char *p = (unsigned const char *)text.c_str() + offset;

	Uint32 result = utf8_next(p);

	offset += utf8_size(p);

	return result;
}

Uint32 utf8_char(std::string text, int i)
{
	unsigned const char *p = (unsigned const char *)text.c_str();

	for (int count = 0; count < i; count++) {
		int size = utf8_size(p);
		if (size == 0) {
			return 0;
		}
		p += size;
	}

	return utf8_next(p);
}

std::string utf8_char_to_string(Uint32 ch)
{
	unsigned char buf[7] = { 0 };

	int bytes;

	if (ch & 0xF7000000) {
		bytes = 6;
	}
	else if (ch & 0x01E00000) {
		bytes = 5;
	}
	else if (ch & 0x001F0000) {
		bytes = 4;
	}
	else if (ch & 0x0000F800) {
		bytes = 3;
	}
	else if (ch & 0x00000780) {
		bytes = 2;
	}
	else {
		bytes = 1;
	}

	if (bytes == 1) {
		buf[0] = (unsigned char)ch;
	}
	else if (bytes == 2) {
		buf[1] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[0] = (ch & 0x1F) | 0xC0;
	}
	else if (bytes == 3) {
		buf[2] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[1] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[0] = (ch & 0xF) | 0xE0;
	}
	else if (bytes == 4) {
		buf[3] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[2] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[1] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[0] = (ch & 0x7) | 0xF0;
	}
	else if (bytes == 5) {
		buf[4] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[3] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[2] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[1] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[0] = (ch & 0x3) | 0xF8;
	}
	else if (bytes == 6) {
		buf[5] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[4] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[3] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[2] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[1] = (ch & 0x3F) | 0x80;
		ch >>= 6;
		buf[0] = (ch & 0x1) | 0xFC;
	}

	return std::string((char *)buf);
}

std::string utf8_substr(std::string s, int start, int len)
{
	std::string result;

	int total_len = utf8_len(s);

	for (int i = start, count = 0; i < total_len && (len == -1 || count < len); i++, count++) {
		result += utf8_char_to_string(utf8_char(s, i));
	}

	return result;
}

}