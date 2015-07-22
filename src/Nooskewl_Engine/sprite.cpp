#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Sprite::Sprite(std::string xml_filename, std::string image_directory, bool absolute_path)
{
	load(xml_filename, image_directory, absolute_path);
	stop();
	reset();
}

Sprite::Sprite(std::string image_directory)
{
	load(image_directory + "/animations.xml", image_directory);
	stop();
	reset();
}

Sprite::~Sprite()
{
	std::map<std::string, Animation *>::iterator it;
	for (it = animations.begin(); it != animations.end(); it++) {
		std::pair<std::string, Animation *> p = *it;
		Animation *a = p.second;
		for (size_t i = 0; i < a->images.size(); i++) {
			delete a->images[i];
		}
		delete a;
	}
}

void Sprite::load(std::string xml_filename, std::string image_directory, bool absolute_path)
{
	if (absolute_path == false) {
		xml_filename = "sprites/" + xml_filename;
		image_directory = "sprites/" + image_directory;
	}

	this->xml_filename = xml_filename;
	this->image_directory = image_directory;

	XML *xml = new XML(xml_filename);

	std::list<XML *> nodes = xml->get_nodes();
	std::list<XML *>::iterator it;

	bool first = true;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		XML *anim = *it;
		int count;
		std::vector<Image *> images;
		for (count = 0; count < 1024 /* NOTE: hardcoded max frames */; count++) {
			std::string filename = image_directory + "/" + anim->get_name() + "/" + itos(count) + ".tga";
			Image *image;
			try {
				image = new Image(filename, true);
			}
			catch (Error e) {
				break;
			}
			images.push_back(image);
		}
		XML *delays = anim->find("delays");
		std::vector<Uint32> delays_vector;
		if (delays == 0) {
			XML *delay = anim->find("delay");
			if  (delay == 0) {
				for (int i = 0; i < count; i++) {
					delays_vector.push_back(100);
				}				
			}
			else {
				Uint32 delay_int = atoi(delay->get_value().c_str());
				for (int i = 0; i < count; i++) {
					delays_vector.push_back(delay_int);
				}
			}
		}
		else {
			for (int i = 0; i < count; i++) {
				XML *delay = delays->find(itos(i));
				if (delay == 0) {
					delays_vector.push_back(100);
				}
				else {
					delays_vector.push_back(atoi(delay->get_value().c_str()));
				}
			}
		}
		Uint32 total_delays = 0;
		for (size_t i = 0; i < delays_vector.size(); i++) {
			total_delays += delays_vector[i];
		}
		Animation *a;
		if (animations.find(anim->get_name()) == animations.end()) {
			a = new Animation();
			a->images = images;
			a->delays = delays_vector;
			a->total_delays = total_delays;
			animations[anim->get_name()] = a;
		}
		else {
			throw Error("Duplicate animation!");
		}
		if (first) {
			first = false;
			current_animation = anim->get_name();
		}
	}

	delete xml;
}

bool Sprite::set_animation(std::string name)
{
	if (animations.find(name) == animations.end()) {
		return false;
	}

	current_animation = name;

	return true;
}

std::string Sprite::get_animation()
{
	return current_animation;
}

void Sprite::start()
{
	if (started == true) {
		return;
	}
	started = true;
	start_time = SDL_GetTicks();
}

void Sprite::stop()
{
	if (started == false) {
		return;
	}
	started = false;
	end_time = SDL_GetTicks();
}

void Sprite::reset()
{
	start_time = SDL_GetTicks();
}

bool Sprite::is_started()
{
	return started;
}

Image *Sprite::get_current_image()
{
	Uint32 now = started ? SDL_GetTicks() : end_time;
	Uint32 elapsed = now - start_time;

	Animation *anim = animations[current_animation];

	Uint32 remainder = (anim->total_delays == 0) ? 0 : (elapsed % anim->total_delays);
	int frame = 0;

	for (size_t i = 0; i < anim->delays.size(); i++) {
		Uint32 delay = anim->delays[i];
		if (remainder >= delay) {
			remainder -= delay;
			frame++;
		}
		else {
			break;
		}
	}

	return anim->images[frame];
}

void Sprite::get_filenames(std::string &xml_filename, std::string &image_directory)
{
	xml_filename = this->xml_filename;
	image_directory = this->image_directory;
}