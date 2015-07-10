// Crystal Picnic Archive: archive format created for the game Crystal Picnic.
// Basically all the files catted together with an index at the end, then
// optionally gzipped.

#ifndef CPA_H
#define CPA_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT CPA
{
public:
	SDL_RWops *load(std::string filename);
	bool exists(std::string filename);

	CPA() throw (Error);
	~CPA();

private:
	unsigned char *bytes;
	std::map< std::string, std::pair<int, int> > info; // offset, size
};

} // End namespace Nooskewl_Engine

#endif