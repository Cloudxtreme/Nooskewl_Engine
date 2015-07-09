#ifndef CPA_H
#define CPA_H

#include "Nooskewl_Engine/main.h"

class EXPORT CPA
{
public:
	SDL_RWops *load(std::string filename);
	bool exists(std::string filename);

	CPA();
	~CPA();

private:
	uint8_t *bytes;
	std::map< std::string, std::pair<int, int> > info; // offset, size
};

#endif