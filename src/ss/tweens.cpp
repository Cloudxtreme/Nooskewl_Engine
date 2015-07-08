#include "ss/tweens.h"

bool SS_Move_Tween::update(Tween *tween)
{
	Uint32 now = SDL_GetTicks();
	Uint32 elapsed = now - tween->start_time;
	float elapsed_f = (float)elapsed / 1000.0f;

	int pixels_moved = int(elapsed_f * speed);

	Point<int> dist = destination - start_pos;
	Size<int> dist_pixels = Size<int>(dist.x*8, dist.y*8);

	if (dist_pixels.length() <= pixels_moved) {
		entity->set_position(destination);
		entity->set_offset(Point<float>(0.0f, 0.0f));
		return false;
	}

	float angle = dist_pixels.angle();

	Point<int> new_pos = Point<int>(int(cos(angle) * pixels_moved), int(sin(angle) * pixels_moved)) + start_pos * 8;
	Point<int> new_tile = new_pos / 8;
	Point<int> offset_pix = new_pos - (new_tile * 8);
	Point<float> offset = offset_pix;
	offset /= 8.0f;

	entity->set_position(new_tile);
	entity->set_offset(offset);

	return true;
}