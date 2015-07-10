/* A very limited "XML" parser (<tag><subtag>value</subtag></tag> basically) */

#include "Nooskewl_Engine/xml.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/util.h"

using namespace Nooskewl_Engine;

XML::XML(std::string name, std::string value) :
	ungot(-1),
	ungotReady(false)
{
	this->name = name;
	this->value = std::string(value);
}

XML::XML(std::string filename) :
	ungot(-1),
	ungotReady(false)

{
	name = std::string("main");

	file = open_file(filename);
	if (file == NULL) {
		throw FileNotFoundError(filename);
	}

	read();

	SDL_RWclose(file);
}

XML::~XML()
{
	std::list<XML*>::iterator it = nodes.begin();

	while (it != nodes.end()) {
		XML* node = *it;
		delete node;
		it++;
	}

	nodes.clear();
}

std::list<XML*> &XML::get_nodes(void)
{
	return nodes;
}

XML* XML::find(std::string name)
{
	std::list<XML*>::iterator it = nodes.begin();

	while (it != nodes.end()) {
		if ((*it)->name == name) {
			return *it;
		}
		it++;
	}

	return 0;
}

std::string XML::get_value()
{
	return value;
}

void XML::add(XML* node)
{
	nodes.push_back(node);
}

std::string XML::get_name(void)
{
	return name;
}

XML::XML(std::string name, SDL_RWops *f) :
	file(f),
	ungot(-1),
	ungotReady(false)
{
	this->name = name;
}

int XML::get()
{
	int c;

	if (ungotReady) {
		c = ungot;
		ungotReady = false;
	}
	else {
		c = SDL_fgetc(file);
	}

	return c;
}

void XML::unget(int c)
{
	ungot = c;
	ungotReady = true;
}

void XML::read()
{
	// read until EOF or end token
	
	for (;;) {
		std::string token;
		token = read_token();
		if (token == "(null)" || (!strncmp(token.c_str(), "</", 2))) {
			return;
		}
		if (token.c_str()[0] == '<') {
#ifdef DEBUG_XML
			std::cout << "Reading sub tag\n";
#endif
			std::string name = get_token_name(token);
#ifdef DEBUG_XML
			std::cout << "Token is " << name.c_str() << "\n";
#endif
			XML* newdata = new XML(name, file);
			newdata->read();
			nodes.push_back(newdata);
		}
		else {
			value += token.c_str();
		}
	}
}

std::string XML::read_token()
{
	int c;

	/* Skip whitespace */

	for (;;) {
		c = get();
		if (c == EOF) {
			return "(null)";
		}
		if (!isspace(c)) {
			break;
		}
	}

	/* Found tag */

	if (c == '<') {
#ifdef DEBUG_XML
		std::cout << "Found tag start/end\n";
#endif
		std::string token;
		token += c;
		for (;;) {
			c = get();
			if (c == EOF) {
				break;
			}
			token += c;
			if (c == '>')
				break;
		}
#ifdef DEBUG_XML
		std::cout << "Read <token>: " << token << "\n";
#endif
		return token;
	}
	/* Found data */
	else {
		std::string data;
		data += c;
		for (;;) {
			c = get();
			if (c == EOF) {
				break;
			}
			if (c == '<') {
				unget(c);
				break;
			}
			data += c;
		}
#ifdef DEBUG_XML
		std::cout << "Read data: " << data << "\n";
#endif
		return data;
	}

	return "(null)";
}

std::string XML::get_token_name(std::string token)
{
	std::string s;
	
	for (int i = 1; token.c_str()[i] != '>' && token.c_str()[i]; i++) {
#ifdef DEBUG_XML
		std::cout << "Read character " << i << "\n";
#endif
		s += token.c_str()[i];
	}

	return s;
}

void XML::write(SDL_RWops *out, int tabs = 0)
{
	char buf[1000];

	if (value == "") {
		for (int i = 0; i < tabs; i++) {
			snprintf(buf, 200, "\t");
			SDL_fputs(buf, out);
		}

		snprintf(buf, 200, "<%s>\n", name.c_str());
		SDL_fputs(buf, out);

		std::list<XML*>::iterator it = nodes.begin();

		while (it != nodes.end()) {
			XML* node = dynamic_cast<XML*>(*it);
			node->write(out, tabs+1);
			it++;
		}

		for (int i = 0; i < tabs; i++) {
			snprintf(buf, 200, "\t");
			SDL_fputs(buf, out);
		}

		snprintf(buf, 200, "</%s>\n", name.c_str());
		SDL_fputs(buf, out);
	}
	else {
		for (int i = 0; i < tabs; i++) {
			snprintf(buf, 200, "\t");
			SDL_fputs(buf, out);
		}

		snprintf(buf, 200, "<%s>", name.c_str());
		SDL_fputs(buf, out);
	
		snprintf(buf, 200, "%s", value.c_str());
		SDL_fputs(buf, out);
		
		std::list<XML*>::iterator it = nodes.begin();

		while (it != nodes.end()) {
			XML* node = dynamic_cast<XML*>(*it);
			node->write(out, tabs+1);
			it++;
		}
		
		snprintf(buf, 200, "</%s>\n", name.c_str());
		SDL_fputs(buf, out);
	}
}