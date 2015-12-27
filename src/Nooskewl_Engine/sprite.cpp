#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

Sprite::Sprite(std::string xml_filename, std::string image_directory, bool absolute_path) :
	started(false),
	previous_animation(""),
	finished_callback(0),
	reverse(false)
{
	load(xml_filename, image_directory, absolute_path);
	start();
}

Sprite::Sprite(std::string image_directory) :
	started(false),
	previous_animation(""),
	finished_callback(0),
	reverse(false)
{
	load(image_directory + "/sprite.xml", image_directory);
	start();
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
			std::string filename = image_directory + "/" + anim->get_name() + itos(count) + ".tga";
			Image *image;
			try {
				image = new Image(filename, true);
			}
			catch (Error e) {
				break;
			}
			images.push_back(image);
		}
		XML *loop_xml = anim->find("loop");
		bool looping;
		if (loop_xml && loop_xml->get_value() == "false") {
			looping = false;
		}
		else {
			looping = true;
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
			a->rand_start = anim->find("rand_start") != 0;
			a->looping = looping;
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

bool Sprite::set_animation(std::string name, Callback finished_callback, void *finished_callback_data)
{
	// Always update these?
	this->finished_callback = finished_callback;
	this->finished_callback_data = finished_callback_data;

	if (current_animation == name) {
		return true;
	}

	if (animations.find(name) == animations.end()) {
		return false;
	}

	previous_animation = current_animation;
	current_animation = name;

	Animation *anim = animations[current_animation];
	if (anim->rand_start) {
		start_time -= rand() % anim->total_delays;
	}

	return true;
}

std::string Sprite::get_animation()
{
	return current_animation;
}

std::string Sprite::get_previous_animation()
{
	return previous_animation;
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

int Sprite::get_length()
{
	return animations[current_animation]->total_delays;
}

void Sprite::set_reverse(bool reverse)
{
	this->reverse = reverse;
}

bool Sprite::is_reversed()
{
	return reverse;
}

Image *Sprite::get_current_image()
{
	Uint32 now = started ? SDL_GetTicks() : end_time;
	Uint32 elapsed = now - start_time;

	Animation *anim = animations[current_animation];

	if (finished_callback != 0 && elapsed >= anim->total_delays) {
		// Back up so you can chain these
		Callback bak_callback = finished_callback;
		void *bak_data = finished_callback_data;
		finished_callback = 0;
		// Don't need to unset data, though it could change
		bak_callback(bak_data);
		// Callback could change these:
		now = started ? SDL_GetTicks() : end_time;
		elapsed = now - start_time;
		anim = animations[current_animation];
	}

	// Don't loop if loop flag off
	if (anim->looping == false && elapsed >= anim->total_delays) {
		return anim->images[anim->images.size()-1];
	}

	Uint32 remainder = (anim->total_delays == 0) ? 0 : (elapsed % anim->total_delays);
	int frame;

	if (reverse) {
		frame = anim->delays.size()-1;
		for (int i = anim->delays.size()-1; i >= 0; i--) {
			Uint32 delay = anim->delays[i];
			if (remainder >= delay) {
				remainder -= delay;
				frame--;
			}
			else {
				break;
			}
		}
	}
	else {
		frame = 0;
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
	}

	return anim->images[frame];
}

void Sprite::get_filenames(std::string &xml_filename, std::string &image_directory)
{
	xml_filename = this->xml_filename;
	image_directory = this->image_directory;
}
