#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"

EXPORT Image *reference_image(std::string filename, bool is_absolute_path = false) throw (Error);
EXPORT void release_image(Image *image);
EXPORT void release_images();
EXPORT void reload_images();

#endif // RESOURCE_MANAGER_H
