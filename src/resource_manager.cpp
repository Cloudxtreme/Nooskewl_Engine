#include "starsquatters.h"
#include "resource_manager.h"

static std::map< std::string, std::pair<int, Image *> > loaded_images;

Image *reference_image(std::string filename, bool is_absolute_path)
{
	std::map< std::string, std::pair<int, Image *> >::iterator it;
	if ((it = loaded_images.find(filename)) != loaded_images.end()) {
		it->second.first++;
		return it->second.second;
	}
	else {
		Image *image;
		try {
			image = new Image(filename, is_absolute_path);
		}
		catch (Error e) {
			throw e;
		}
		loaded_images[filename] = std::pair<int, Image *>(1, image);
		return image;
	}
}

void release_image(Image *image)
{
	std::map< std::string, std::pair<int, Image *> >::iterator it = loaded_images.find(image->filename);
	if (it == loaded_images.end()) {
		return;
	}
	it->second.first--;
	if (it->second.first <= 0) {
		delete it->second.second;
		loaded_images.erase(it);
	}
}