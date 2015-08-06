#ifndef XML_H
#define XML_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT XML {
public:
	XML(std::string name, std::string value);
	XML(std::string filename);
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

namespace XML_Helpers {
	XML *handle_rand_tag(XML *xml);
	void handle_min_max_tag(XML *xml, int32_t &min, int32_t &max);
	void handle_randn_tag(XML *xml, uint32_t &ret);
	int32_t handle_numeric_tag(XML *xml);
}

} // End namespace Nooskewl_Engine

#endif // XML_H
