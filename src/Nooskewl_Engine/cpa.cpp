#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/internal.h"

using namespace Nooskewl_Engine;

static Uint8 *safe_find_char(Uint8 *haystack, char needle, Uint8 *end)
{
	Uint8 *p = haystack;
	while (p != end) {
		if (*p == needle) {
			return p;
		}
		p++;
	}
	return NULL;
}

SDL_RWops *CPA::open(std::string filename)
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

CPA::CPA()
{
	List_Directory ld(".\\*.cpa");
	std::string filename;
	SDL_RWops *file;

	while ((filename = ld.next()) != "") {
		file = SDL_RWFromFile(filename.c_str(), "rb");
		if (file != NULL) {
			break;
		}
	}

	if (file) {
		// The last 4 bytes of every gzipped file is the size of the uncompressed data as a 32 bit little endian number
		SDL_RWseek(file, -4, RW_SEEK_END);
		int size = SDL_ReadLE32(file);
		SDL_RWclose(file);

		gzFile f = gzopen(filename.c_str(), "rb");
		bytes = new Uint8[size];
		int read = gzread(f, bytes, size);
		gzclose(f);

		if (read != size) {
			throw Error("Invalid CPA: corrupt gzip file");
		}

		// Read the size of the data (ascii text followed by newline first thing in the file)
		Uint8 *header_end = (Uint8 *)safe_find_char(bytes, '\n', bytes+size);
		if (header_end == NULL) {
			throw Error("Invalid CPA: header not present");
		}
		int header_size = (header_end - bytes) + 1;
		int data_size = atoi((char *)bytes);
		if (data_size >= size) {
			throw Error("Invalid CPA: data size > archive size");
		}
		// Skip to the info section at the end
		Uint8 *p = bytes + header_size + data_size;
		// Keep track of the byte offset of each file
		int count = header_size;

		int total_size = 0;

		char line[1000];

		while (p < bytes+size) {
			Uint8 *end = (Uint8 *)safe_find_char(p, '\n', bytes+size);
			if (end == NULL) {
				throw Error("Invalid CPA: corrupt info section");
			}
			int len = end-p;
			if (len < 1000) {
				memcpy(line, p, len);
				line[len] = 0;
				char size_text[1000];
				char name_text[1000];
				Uint8 *size_text_end = (Uint8 *)safe_find_char(p, '\t', bytes+size);
				if (size_text_end == NULL || size_text_end - p > 999) {
					throw Error("Invalid CPA: corrupt info section");
				}
				memcpy(size_text, p, size_text_end-p);
				size_text[size_text_end-p] = 0;
				memcpy(name_text, size_text_end+1, end-size_text_end-1);
				name_text[end-size_text_end-1] = 0;
				int file_size = atoi(size_text);
				std::pair<int, int> pair(count, file_size);
				info[name_text] = pair;
				count += file_size;
				total_size += file_size;
			}
			p += len + 1;
		}

		if (total_size > data_size) {
			throw Error("Invalid CPA: total file sizes > data size");
		}
	}
}

CPA::~CPA()
{
	delete[] bytes;
}
