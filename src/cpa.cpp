#include <zlib.h>

#include "cpa.h"

CPA *cpa;

SDL_RWops *CPA::load(std::string filename)
{
	if (!exists(filename)) {
		return NULL;
	}
	return SDL_RWFromMem(bytes+info[filename].first, info[filename].second);
}

bool CPA::exists(std::string filename)
{
	return info.find(filename) != info.end();
}

CPA::CPA(std::string archive_name)
{
	SDL_RWops *file = SDL_RWFromFile(archive_name.c_str(), "rb");

	if (file) {
		SDL_RWseek(file, -4, RW_SEEK_END);
		int sz = SDL_ReadLE32(file);
		SDL_RWclose(file);

		gzFile f = gzopen(archive_name.c_str(), "rb");
		bytes = new uint8_t[sz];
		gzread(f, bytes, sz);
		gzclose(f);
		int count;

		int header_sz = (strchr((const char *)bytes, '\n') - (char *)bytes) + 1;
		int data_sz = atoi((const char *)bytes);

		uint8_t *p = bytes + header_sz + data_sz;
		count = header_sz;

		char str[1000];

		while (p < bytes+sz) {
			const char *end = strchr((const char *)p, '\n');
			int len = end-(char *)p;
			memcpy(str, p, len);
			str[len] = 0;
			char size[1000];
			char name[1000];
			sscanf(str, "%s %s", size, name);
			std::pair<int, int> pair(count, atoi(size));
			info[name] = pair;
			count += atoi(size);
			p += len + 1;
		}
	}
	else {
		bytes = NULL;
	}
}

CPA::~CPA()
{
	delete[] bytes;
}