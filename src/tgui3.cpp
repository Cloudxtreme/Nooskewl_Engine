#include "tgui3.h"

TGUI::TGUI(TGUI_Widget *main_div, int w, int h) :
	main_div(main_div),
	w(w),
	h(h),
	focus(NULL),
	offset_x(0),
	offset_y(0)
{
	layout();
	focus_something();
}

void TGUI::layout()
{
	set_sizes(main_div);
	set_positions(main_div, offset_x, offset_y);
}

void TGUI::resize(int w, int h)
{
	this->w = w;
	this->h = h;
	layout();
}

void TGUI::draw()
{
	draw(main_div);
}

void TGUI::handle_event(TGUI_Event *event)
{
	int x = 0;
	int y = 0;
	if (event->type == TGUI_JOY_AXIS && fabs(event->joystick.value) > 0.25f) {
		if (event->joystick.axis == 0) {
			if (event->joystick.value < 0) {
				x = -1;
			}
			else {
				x = 1;
			}
		}
		else if (event->joystick.axis == 1) {
			if (event->joystick.value < 0) {
				y = -1;
			}
			else {
				y = 1;
			}
		}
	}
	else if (event->type == TGUI_KEY_DOWN) {
		if (event->keyboard.code == TGUIK_LEFT) {
			x = -1;
		}
		else if (event->keyboard.code == TGUIK_RIGHT) {
			x = 1;
		}
		else if (event->keyboard.code == TGUIK_UP) {
			y = -1;
		}
		else if (event->keyboard.code == TGUIK_DOWN) {
			y = 1;
		}
	}

	if (x == 0 && y == 0) {
		handle_event(event, main_div);
	}
	else {
		TGUI_Widget *start = focus == NULL ? main_div : focus;
		TGUI_Widget *best = start;
		int best_score = INT_MAX;
		int best_grade = 2;
		find_focus(start, best, main_div, x, y, best_score, best_grade);
		focus = best;
	}
}

void TGUI::set_focus(TGUI_Widget *div)
{
	focus = div;
}

void TGUI::focus_something()
{
	focus_something(main_div);
}

void TGUI::set_offset(int offset_x, int offset_y)
{
	this->offset_x = offset_x;
	this->offset_y = offset_y;
	layout();
}

TGUI_Widget *TGUI::get_focus()
{
	return focus;
}

TGUI_Widget *TGUI::get_event_owner(TGUI_Event *event)
{
	return get_event_owner(event, main_div);
}

void TGUI::set_sizes(TGUI_Widget *div)
{
	div->gui = this;
	int width, height;
	tgui_get_size(div->parent, div, &width, &height);
	div->calculated_w = width;
	div->calculated_h = height;
	for (size_t i = 0; i < div->children.size(); i++) {
		set_sizes(div->children[i]);
	}
}

void TGUI::set_positions(TGUI_Widget *div, int x, int y)
{
	div->calculated_x = x;
	div->calculated_y = y;

	int parent_width, parent_height;

	if (div->parent) {
		parent_width = div->parent->calculated_w + div->parent->padding_left + div->parent->padding_right;
		parent_height = div->parent->calculated_h + div->parent->padding_top + div->parent->padding_bottom;
	}
	else {
		parent_width = w;
		parent_height = h;
	}

	int max_h = 0;
	int dx = x;
	int dy = y;

	for (size_t i = 0; i < div->children.size(); i++) {
		TGUI_Widget *d = div->children[i];

		int width = d->calculated_w + d->padding_left + d->padding_right;
		int height = d->calculated_h + d->padding_top + d->padding_bottom;

		if (dx + width > parent_width) {
			dx = x;
			dy += max_h;
			max_h = 0;
		}

		set_positions(d, dx+d->padding_left+d->get_right_pos(), dy+d->padding_top);

		if (d->float_right == false) {
			dx += width;
		}

		max_h = height > max_h ? height : max_h;
	}
}

void TGUI::draw(TGUI_Widget *div)
{
	div->draw();
	for (size_t i = 0; i < div->children.size(); i++) {
		draw(div->children[i]);
	}
}

TGUI_Widget *TGUI::get_event_owner(TGUI_Event *event, TGUI_Widget *div)
{
	for (size_t i = 0; i < div->children.size(); i++) {
		TGUI_Widget *d = get_event_owner(event, div->children[i]);
		if (d != NULL) {
			return d;
		}
	}

	if (event->type == TGUI_MOUSE_DOWN || event->type == TGUI_MOUSE_UP || event->type == TGUI_MOUSE_AXIS) {
		if (event->mouse.x >= div->calculated_x && event->mouse.x < div->calculated_x+div->calculated_w && event->mouse.y >= div->calculated_y && event->mouse.y < div->calculated_y+div->calculated_h) {
			return div;
		}
	}
	else if (event->type != TGUI_UNKNOWN && div == focus) {
		return div;
	}

	return NULL;
}

void TGUI::handle_event(TGUI_Event *event, TGUI_Widget *div)
{
	div->handle_event(event);

	for (size_t i = 0; i < div->children.size(); i++) {
		handle_event(event, div->children[i]);
	}
}

bool TGUI::focus_something(TGUI_Widget *div)
{
	if (div->accepts_focus) {
		focus = div;
		return true;
	}

	for (size_t i = 0; i < div->children.size(); i++) {
		if (focus_something(div->children[i])) {
			return true;
		}
	}

	return false;
}

// Returns positive for aligned match, negative for unaligned match
void TGUI::focus_distance(TGUI_Widget *start, TGUI_Widget *div, int dir_x, int dir_y, int &score, int &grade)
{
	int cx = start->calculated_x + start->calculated_w / 2;
	int cy = start->calculated_y + start->calculated_h / 2;

	int box_x1 = cx;
	int box_x2 = cx + dir_x * 1000000;
	int box_y1 = cy;
	int box_y2 = cy + dir_y * 1000000;

	if (box_x1 == box_x2) {
		box_x1 = start->calculated_x;
		box_x2 = box_x1 + start->calculated_w;
	}
	else {
		box_y1 = start->calculated_y;
		box_y2 = box_y1 + start->calculated_h;
	}

	if (box_x2 < box_x1) {
		int tmp = box_x1;
		box_x1 = box_x2;
		box_x2 = tmp;
	}

	if (box_y2 < box_y1) {
		int tmp = box_y1;
		box_y1 = box_y2;
		box_y2 = tmp;
	}

	int div_x1 = div->calculated_x;
	int div_x2 = div_x1 + div->calculated_w;
	int div_y1 = div->calculated_y;
	int div_y2 = div_y1 + div->calculated_h;

	int div_cx = (div_x1 + div_x2) / 2;
	int div_cy = (div_y1 + div_y2) / 2;

	int dx = div_cx - cx;
	int dy = div_cy - cy;
	int dist = int(sqrtf(float(dx*dx + dy*dy)));

	if (!(div_x1 > box_x2 || div_x2 < box_x1 || div_y1 > box_y2 || div_y2 < box_y1)) {
		grade = 0;
	}
	else if (dir_x < 0 && div_cx < cx || dir_x > 0 && div_cx > cx || dir_y < 0 && div_cy < cy || dir_y > 0 && div_cy > cy) {
		grade = 1;
	}
	else {
		grade = 2;
	}

	score = dist;
}

void TGUI::find_focus(TGUI_Widget *start, TGUI_Widget *&current_best, TGUI_Widget *div, int dir_x, int dir_y, int &best_score, int &best_grade)
{
	if (div->accepts_focus && div != start) {
		int score, grade;
		focus_distance(start, div, dir_x, dir_y, score, grade);
		if (grade < best_grade) {
			best_score = score;
			best_grade = grade;
			current_best = div;
		}
		else if (grade == best_grade && score < best_score) {
			best_score = score;
			best_grade = grade;
			current_best = div;
		}
	}

	for (size_t i = 0; i < div->children.size(); i++) {
		find_focus(start, current_best, div->children[i], dir_x, dir_y, best_score, best_grade);
	}
}

TGUI_Widget::TGUI_Widget(int w, int h) :
	parent(NULL),
	percent_x(false),
	percent_y(false),
	w(w),
	h(h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

TGUI_Widget::TGUI_Widget(float percent_w, float percent_h) :
	parent(NULL),
	percent_x(true),
	percent_y(true),
	percent_w(percent_w),
	percent_h(percent_h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

TGUI_Widget::TGUI_Widget(int w, float percent_h) :
	parent(NULL),
	percent_x(false),
	percent_y(true),
	percent_h(percent_h),
	w(w),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

TGUI_Widget::TGUI_Widget(float percent_w, int h) :
	parent(NULL),
	percent_x(true),
	percent_y(false),
	percent_w(percent_w),
	h(h),
	padding_left(0),
	padding_right(0),
	padding_top(0),
	padding_bottom(0),
	float_right(false),
	accepts_focus(false)
{
}

void TGUI_Widget::set_parent(TGUI_Widget *div)
{
	parent = div;
	parent->children.push_back(this);
}

void TGUI_Widget::set_padding(int padding)
{
	padding_left = padding_right = padding_top = padding_bottom = padding;
}

void TGUI_Widget::set_padding(int left, int right, int top, int bottom)
{
	padding_left = left;
	padding_right = right;
	padding_top = top;
	padding_bottom = bottom;
}

void TGUI_Widget::set_float_right(bool float_right)
{
	this->float_right = float_right;
}

void TGUI_Widget::set_accepts_focus(bool accepts_focus)
{
	this->accepts_focus = accepts_focus;
	if (accepts_focus == false) {
		if (gui->get_focus() == this) {
			gui->focus_something();
		}
	}
}

TGUI_Widget *TGUI_Widget::get_parent()
{
	return parent;
}

int TGUI_Widget::get_x()
{
	return calculated_x;
}

int TGUI_Widget::get_y()
{
	return calculated_y;
}

int TGUI_Widget::get_width()
{
	return calculated_w;
}

int TGUI_Widget::get_height()
{
	return calculated_h;
}

int TGUI_Widget::get_padding_left()
{
	return padding_left;
}

int TGUI_Widget::get_padding_right()
{
	return padding_right;
}

int TGUI_Widget::get_padding_top()
{
	return padding_top;
}

int TGUI_Widget::get_padding_bottom()
{
	return padding_bottom;
}

int TGUI_Widget::get_right_pos()
{
	if (float_right == false) {
		return 0;
	}
	int parent_width;
	tgui_get_size(parent->parent, parent, &parent_width, NULL);
	parent_width += parent->padding_left + parent->padding_right;
	int width;
	tgui_get_size(parent, this, &width, NULL);
	width += padding_left + padding_right;
	int right = 0;
	for (size_t i = 0; i < parent->children.size(); i++) {
		TGUI_Widget *d = parent->children[i];
		if (d == this) {
			break;
		}
		if (d->float_right) {
			int w2;
			tgui_get_size(parent, d, &w2, NULL);
			w2 += d->padding_left + d->padding_right;
			right += w2;
		}
	}
	return parent_width - (right + width);
}

void tgui_get_size(TGUI_Widget *parent, TGUI_Widget *div, int *width, int *height)
{
	if (parent == NULL) {
		*width = div->gui->w;
		*height = div->gui->h;
	}
	else {
		int w, h;
		tgui_get_size(parent->parent, parent, &w, &h);
		w += parent->padding_left + parent->padding_right;
		h += parent->padding_top + parent->padding_bottom;
		if (width) {
			if (div->percent_x) {
				if (div->percent_w < 0) {
					int total_w = 0;
					float total_percent = 0.0f;
					for (size_t i = 0; i < parent->children.size(); i++) {
						int this_w = 0;
						TGUI_Widget *d = parent->children[i];
						if (d->percent_x) {
							if (d->percent_w < 0) {
								total_percent += -d->percent_w;
							}
							else {
								int w2;
								tgui_get_size(parent, d, &w2, NULL);
								w2 += d->padding_left + d->padding_right;
								this_w = w2;
							}
						}
						else {
							this_w = d->w + d->padding_left + d->padding_right;
						}
						if (total_w + this_w > w) {
							total_w = 0;
						}
						if (d->float_right == false) {
							total_w += this_w;
						}
						if (d == div) {
							break;
						}
					}
					int remainder = w - total_w;
					if (remainder > 0) {
						*width = remainder * int(-div->percent_w / total_percent) - (div->padding_left + div->padding_right);
					}
					else {
						*width = 0;
					}
				}
				else {
					*width = int(w * div->percent_w);
				}
			}
			else {
				*width = div->w;
			}
		}
		if (height) {
			if (div->percent_y) {
				if (div->percent_h < 0) {
					int total_w = 0;
					int total_h = 0;
					float total_percent = 0.0f;
					int max_h = 0;
					float max_percent = 0.0f;
					for (size_t i = 0; i < parent->children.size(); i++) {
						int this_w = 0;
						int this_h = 0;
						float this_percent = 0.0f;
						TGUI_Widget *d = parent->children[i];
						tgui_get_size(parent, d, &this_w, NULL);
						this_w += d->padding_left + d->padding_right;
						if (d->percent_y) {
							if (d->percent_h < 0) {
								this_percent = -d->percent_h;
							}
							else {
								int h2;
								tgui_get_size(parent, d, NULL, &h2);
								h2 += d->padding_top + d->padding_bottom;
								this_h = h2;
							}
						}
						else {
							this_h = d->h + d->padding_top + d->padding_bottom;
						}
						if (total_w + this_w <= w) {
							if (this_h > max_h) {
								max_h = this_h;
							}
							if (this_percent > max_percent) {
								max_percent = this_percent;
							}
						}
						if (total_w + this_w >= w) {
							total_h += max_h;
							total_percent += max_percent;
							if (total_w + this_w > w) {
								max_h = this_h;
								max_percent = this_percent;
								total_w = this_w;
							}
							else {
								max_h = 0;
								max_percent = 0.0f;
								total_w = 0;
							}
						}
						else if (i == parent->children.size()-1) {
							total_percent += max_percent;
						}
						else if (d->float_right == false) {
							total_w += this_w;
						}
					}
					int remainder = h - total_h;
					if (remainder > 0) {
						*height = remainder * int(-div->percent_h / total_percent) - (div->padding_top + div->padding_bottom);
					}
					else {
						*height = 0;
					}
				}
				else {
					*height = int(h * div->percent_h);
				}
			}
			else {
				*height = div->h;
			}
		}
	}
}

TGUI_Event tgui_get_relative_event(TGUI_Widget *div, TGUI_Event *event)
{
	TGUI_Event new_event = *event;

	if (new_event.type == TGUI_MOUSE_DOWN || new_event.type == TGUI_MOUSE_UP || new_event.type == TGUI_MOUSE_AXIS) {
		new_event.mouse.x -= div->get_x();
		new_event.mouse.y -= div->get_y();
	}

	return new_event;
}

#ifdef WITH_SDL
TGUI_Event tgui_sdl_convert_event(SDL_Event *sdl_event)
{
	TGUI_Event event;

	switch (sdl_event->type) {
		case SDL_KEYDOWN:
			event.type = TGUI_KEY_DOWN;
			event.keyboard.code = sdl_event->key.keysym.sym;
			break;
		case SDL_KEYUP:
			event.type = TGUI_KEY_UP;
			event.keyboard.code = sdl_event->key.keysym.sym;
			break;
		case SDL_MOUSEBUTTONDOWN:
			event.type = TGUI_MOUSE_DOWN;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = sdl_event->button.x;
			event.mouse.y = sdl_event->button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			event.type = TGUI_MOUSE_UP;
			event.mouse.button = sdl_event->button.button;
			event.mouse.x = sdl_event->button.x;
			event.mouse.y = sdl_event->button.y;
			break;
		case SDL_MOUSEMOTION:
			event.type = TGUI_MOUSE_AXIS;
			event.mouse.button = -1;
			event.mouse.x = sdl_event->motion.x;
			event.mouse.y = sdl_event->motion.y;
			break;
		case SDL_JOYBUTTONDOWN:
			event.type = TGUI_JOY_DOWN;
			event.joystick.button = sdl_event->jbutton.button;
			event.joystick.axis = -1;
			event.joystick.value = 0.0f;
			break;
		case SDL_JOYBUTTONUP:
			event.type = TGUI_JOY_UP;
			event.joystick.button = sdl_event->jbutton.button;
			event.joystick.axis = -1;
			event.joystick.value = 0.0f;
			break;
		case SDL_JOYAXISMOTION:
			event.type = TGUI_JOY_AXIS;
			event.joystick.button = -1;
			event.joystick.axis = sdl_event->jaxis.axis;
			event.joystick.value = float(sdl_event->jaxis.value + 32768) / 65535.0f * 2.0f - 1.0f;
			break;
		default:
			event.type = TGUI_UNKNOWN;
			break;
	}

#ifdef TGUI_DEBUG
	switch (event.type) {
		case TGUI_KEY_DOWN:
		case TGUI_KEY_UP:
			printf("[%2d] %d\n", event.type, event.keyboard.code);
			break;
		case TGUI_MOUSE_DOWN:
		case TGUI_MOUSE_UP:
		case TGUI_MOUSE_AXIS:
			printf("[%2d] %d %d,%d\n", event.type, event.mouse.button, event.mouse.x, event.mouse.y);
			break;
		case TGUI_JOY_DOWN:
		case TGUI_JOY_UP:
		case TGUI_JOY_AXIS:
			printf("[%2d] %d %d %f\n", event.type, event.joystick.button, event.joystick.axis, event.joystick.value);
			break;
	}
#endif

	return event;
}
#endif