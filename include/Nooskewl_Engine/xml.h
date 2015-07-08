#ifndef XML_H
#define XML_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"

class EXPORT XML {
public:
	XML(std::string name, std::string value);
	XML(std::string filename) throw (Error);
	~XML();

	std::list<XML *> &get_nodes();
	XML *find(std::string name);
	std::string get_value();
	void add(XML *node);
	std::string get_name();
	bool failed();

protected:
	XML(std::string name, SDL_RWops *f);

	int get();
	void unget(int c);
	void read();
	std::string read_token();
	std::string get_token_name(std::string token);
	void write(SDL_RWops *out, int tab);

	SDL_RWops *file;
	std::string name;
	std::string value;
	std::list<XML *> nodes;
	bool debug;
	int ungot;
	bool ungotReady;
};

#endif // XML_H
