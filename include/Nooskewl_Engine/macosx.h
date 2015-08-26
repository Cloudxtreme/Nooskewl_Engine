#ifndef MACOSX_H
#define MACOSX_H

#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

void macosx_create_custom_cursor(Uint8 *data, Size<int> size, int x_focus, int y_focus);
void macosx_set_custom_cursor();
void macosx_destroy_custom_cursor();

} // End namespace Nooskewl_Engine

#endif
