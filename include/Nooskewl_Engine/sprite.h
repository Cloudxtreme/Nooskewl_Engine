#ifndef SPRITE_H
#define SPRITE_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Image;

class NOOSKEWL_ENGINE_EXPORT Sprite {
public:
	Sprite(std::string xml_filename, std::string image_directory, bool absolute_path = false);
	Sprite(std::string image_directory);
	~Sprite();

	bool set_animation(std::string name, Callback finished_callback = 0, void *finished_callback_data = 0);
	std::string get_animation();
	std::string get_previous_animation();

	void start();
	void stop();
	// set to frame 0
	void reset();
	bool is_started();
	int get_length();

	Image *get_current_image();

	void get_filenames(std::string &xml_filename, std::string &image_directory);

private:
	struct Animation {
		std::vector<Image *> images;
		std::vector<Uint32> delays;
		Uint32 total_delays;
		bool rand_start;
	};

	void load(std::string xml_filename, std::string image_directory, bool absolute_path = false);

	bool started;
	Uint32 start_time;
	Uint32 end_time;

	// "" when not set
	std::string current_animation;
	std::string previous_animation;
	std::map<std::string, Animation *> animations;

	std::string xml_filename;
	std::string image_directory;

	Callback finished_callback;
	void *finished_callback_data;
};

} // End namespace Nooskewl_Engine

#endif // SPRITE_H
