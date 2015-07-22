#ifndef SPRITE_H
#define SPRITE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Sprite {
public:
	Sprite(std::string xml_filename, std::string image_directory, bool absolute_path = false);
	Sprite(std::string image_directory);
	~Sprite();

	bool set_animation(std::string name);

	void start();
	void stop();
	// set to frame 0
	void reset();

	Image *get_current_image();

	void get_filenames(std::string &xml_filename, std::string &image_directory);

private:
	struct Animation {
		std::vector<Image *> images;
		std::vector<Uint32> delays;
		Uint32 total_delays;
	};

	void load(std::string xml_filename, std::string image_directory, bool absolute_path = false);

	bool started;
	Uint32 start_time;
	Uint32 end_time;

	// "" when not set
	std::string current_animation;
	std::map<std::string, Animation *> animations;

	std::string xml_filename;
	std::string image_directory;
};

} // End namespace Nooskewl_Engine

#endif // SPRITE_H
