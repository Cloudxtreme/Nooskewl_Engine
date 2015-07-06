#include "starsquatters.h"
#include "resource_manager.h"

static std::map<std::string, Image *> loaded_images;

Image *reference_image(std::string filename, bool is_absolute_path)
{
	std::map<std::string, Image *>::iterator it;
	if ((it = loaded_images.find(filename)) != loaded_images.end()) {
		return it->second;
	}
	else {
		Image *image;
		try {
			image = new Image(filename, is_absolute_path);
		}
		catch (Error e) {
			throw e;
		}
		loaded_images[filename] = image;
		return image;
	}
}

void release_image(Image *image)
{
	std::map<std::string, Image *>::iterator it = loaded_images.find(image->filename);
	if (it == loaded_images.end()) {
		return;
	}
	delete it->second;
	loaded_images.erase(it);
}