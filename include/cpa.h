#ifndef CPA_H
#define CPA_H

#include "starsquatters.h"

class CPA
{
public:
	SDL_RWops *load(std::string filename);
	bool exists(std::string filename);

	CPA(std::string archive_name);
	~CPA();

private:
	uint8_t *bytes;
	std::map< std::string, std::pair<int, int> > info; // offset, size
};

extern CPA *cpa;

#endif