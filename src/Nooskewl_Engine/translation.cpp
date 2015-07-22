#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/translation.h"

using namespace Nooskewl_Engine;

Translation::Translation(std::string text)
{
	SDL_RWops *file = SDL_RWFromMem((void *)text.c_str(), text.length());

	if (file) {
		const int max = 5000;
		char line[max];

		while (SDL_fgets(file, line, max)) {
			if (line[strlen(line)-1] == '\r' || line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
			if (line[strlen(line)-1] == '\r' || line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
			const char *p = strchr(line, ':');
			if (p) {
				int num = atoi(line);
				p++;
				translation[num] = p;
			}
		}
	}

	SDL_RWclose(file);
}


std::string Translation::translate(int id)
{
	std::map<int, std::string>::iterator it;
	if ((it = translation.find(id)) != translation.end()) {
		return (*it).second;
	}
	return "X";
}
