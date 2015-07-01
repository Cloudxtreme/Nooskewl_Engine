#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "image.h"

Image *reference_image(std::string filename);
void release_image(Image *image);

#endif // RESOURCE_MANAGER_H