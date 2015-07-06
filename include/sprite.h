#ifndef SPRITE_H
#define SPRITE_H

#include "starsquatters.h"
#include "error.h"
#include "image.h"

class Sprite {
public:
	Sprite(std::string xml_filename, std::string image_directory) throw (Error);
	Sprite(std::string directory_name) throw (Error);
	~Sprite();

	bool set_animation(std::string name);

	void start();
	void stop();
	// set to frame 0
	void reset();
	void update();

	Image *get_current_image();

private:
	struct Animation {
		std::vector<Image *> images;
		std::vector<Uint32> delays;
		Uint32 total_delays;
	};

	bool started;
	Uint32 start_time;

	// "" when not set
	std::string current_animation;

	std::map<std::string, Animation *> animations;

	Image *current_image;
};

#endif // SPRITE_H