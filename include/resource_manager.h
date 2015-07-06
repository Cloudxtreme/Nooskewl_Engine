#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "error.h"
#include "image.h"

Image *reference_image(std::string filename, bool is_absolute_path = false) throw (Error);
void release_image(Image *image);

#endif // RESOURCE_MANAGER_H