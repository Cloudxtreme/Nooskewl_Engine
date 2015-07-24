#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/tilemap.h"

using namespace Nooskewl_Engine;

static int current_id;

void Map_Entity::new_game_started()
{
	current_id = 0;
}

Map_Entity::Map_Entity(std::string name) :
	name(name),
	direction(S),
	sprite(0),
	position(0, 0),
	moving(false),
	speed(0.1f),
	offset(0.0f, 0.0f),
	bounce(1),
	solid(true),
	size(noo.tile_size, noo.tile_size*2),
	stop_next_tile(false),
	sitting(false),
	input_disabled(false),
	z_add(0),
	following_path(false),
	path_callback(0)
{
	id = current_id++;
}

Map_Entity::~Map_Entity()
{
	delete brain;
	delete sprite;
}

void Map_Entity::set_brain(Brain *brain)
{
	this->brain = brain;
	if (brain) {
		brain->set_map_entity(this);
	}
}

void Map_Entity::load_sprite(std::string name)
{
	delete sprite;
	sprite = new Sprite(name + "/animations.xml", name);
	sprite->set_animation("stand_s");
}

void Map_Entity::set_sprite(Sprite *sprite)
{
	delete this->sprite;
	this->sprite = sprite;
}

void Map_Entity::set_position(Point<int> position)
{
	this->position = position;
}

void Map_Entity::set_offset(Point<float> offset)
{
	this->offset = offset;
}

void Map_Entity::set_bounce(int bounce)
{
	this->bounce = bounce;
}

void Map_Entity::set_direction(Direction direction)
{
	this->direction = direction;
	std::string animation_name;
	if (moving) {
		switch (direction) {
			case N:
				animation_name = "walk_n";
				break;
			case E:
				animation_name = "walk_e";
				break;
			case S:
				animation_name = "walk_s";
				break;
			default:
				animation_name = "walk_w";
				break;
		}
	}
	else if (sitting) {
		switch (direction) {
			case N:
				animation_name = "sit_n";
				break;
			case E:
				animation_name = "sit_e";
				break;
			case S:
				animation_name = "sit_s";
				break;
			default:
				animation_name = "sit_w";
				break;
		}
	}
	else {
		switch (direction) {
			case N:
				animation_name = "stand_n";
				break;
			case E:
				animation_name = "stand_e";
				break;
			case S:
				animation_name = "stand_s";
				break;
			default:
				animation_name = "stand_w";
				break;
		}
	}

	sprite->set_animation(animation_name);
}

void Map_Entity::set_solid(bool solid)
{
	this->solid = solid;
}

void Map_Entity::set_sitting(bool sitting)
{
	this->sitting = sitting;
	if (sitting) {
		moving = false;
	}
	set_direction(direction);
}

void Map_Entity::set_path(std::list<A_Star::Node *> path, Callback callback, void *callback_data)
{
	if (path.size() > 0) {
		this->path = path;
		following_path = true;
		path_callback = callback;
		path_callback_data = callback_data;
		follow_path();
	}
}

// This can fix depth issues for sprites that overhang Object-grouped tiles
void Map_Entity::set_z_add(int z_add)
{
	this->z_add = z_add;
}

int Map_Entity::get_id()
{
	return id;
}

std::string Map_Entity::get_name()
{
	return name;
}

Brain *Map_Entity::get_brain()
{
	return brain;
}

Sprite *Map_Entity::get_sprite()
{
	return sprite;
}

Direction Map_Entity::get_direction()
{
	return direction;
}

Point<int> Map_Entity::get_position()
{
	return position;
}

Point<float> Map_Entity::get_offset()
{
	return offset;
}

Size<int> Map_Entity::get_size()
{
	return size;
}

Point<float> Map_Entity::get_draw_position()
{
		int h = sprite->get_current_image()->size.h;
		return Point<float>(position.x*noo.tile_size+(offset.x*(float)noo.tile_size)+(noo.tile_size-sprite->get_current_image()->size.w)/2, (position.y+1)*noo.tile_size+(offset.y*(float)noo.tile_size)-h);
}

bool Map_Entity::is_solid()
{
	return solid;
}

bool Map_Entity::is_sitting()
{
	return sitting;
}

int Map_Entity::get_z_add()
{
	return z_add;
}

bool Map_Entity::pixels_collide(Point<int> position, Size<int> size)
{
	Point<int> pos = this->position * noo.tile_size + this->offset * (float)noo.tile_size;
	if (pos.x >= position.x+size.w || pos.x+this->size.w <= position.x || pos.y >= position.y+size.h || pos.y+this->size.h <= position.y) {
		return false;
	}
	return true;
}

void Map_Entity::disable_input()
{
	input_disabled = true;
}

void Map_Entity::enable_input()
{
	input_disabled = false;
}

bool Map_Entity::tiles_collide(Point<int> position, Size<int> size, Point<int> &collide_pos)
{
	if (this->position.x >= position.x && this->position.x < position.x+size.w && this->position.y >= position.y && this->position.y < position.y+size.h) {
		collide_pos = Point<int>(this->position.x-position.x, this->position.y-position.y);
		return true;
	}
	return false;
}

void Map_Entity::stop()
{
	if (following_path == false && moving) {
		stop_next_tile = true;
	}
	else {
		stop_now();
	}
}

bool Map_Entity::maybe_move()
{
	bool ret = false;

	if ((following_path || input_disabled == false) && brain) {
		if (brain->l) {
			if (!sitting && noo.map->is_solid(-1, position + Point<int>(-1, 0), Size<int>(1, 1)) == false) {
				moving = true;
				offset = Point<float>(1, 0);
				position += Point<int>(-1, 0);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			set_direction(W);
		}
		else if (brain->r) {
			if (!sitting && noo.map->is_solid(-1, position + Point<int>(1, 0), Size<int>(1, 1)) == false) {
				moving = true;
				direction = E;
				offset = Point<float>(-1, 0);
				position += Point<int>(1, 0);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			set_direction(E);
		}
		else if (brain->u) {
			if (!sitting && noo.map->is_solid(-1, position + Point<int>(0, -1), Size<int>(1, 1)) == false) {
				moving = true;
				direction = N;
				offset = Point<float>(0, 1);
				position += Point<int>(0, -1);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			set_direction(N);
		}
		else if (brain->d) {
			if (!sitting && noo.map->is_solid(-1, position + Point<int>(0, 1), Size<int>(1, 1)) == false) {
				moving = true;
				direction = S;
				offset = Point<float>(0, -1);
				position += Point<int>(0, 1);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			set_direction(S);
		}
		else if (brain->b1 && sitting) {
			sitting = false;
			set_direction(direction);
		}
	}

	return ret;
}

void Map_Entity::handle_event(TGUI_Event *event)
{
	if (following_path == false && brain) {
		brain->handle_event(event);
	}
}

bool Map_Entity::update(bool can_move)
{
	if (moving == false) {
		maybe_move();
	}

	if (moving) {
		if (offset.x < 0) {
			offset.x += speed;
			if (offset.x >= 0) {
				offset.x = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else {
					if (following_path) {
						follow_path();
					}
					else if (maybe_move() == false) {
						moving = false;
						sprite->set_animation("stand_e");
					}
				}
			}
		}
		else if (offset.x > 0) {
			offset.x -= speed;
			if (offset.x <= 0) {
				offset.x = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else {
					if (following_path) {
						follow_path();
					}
					else if (maybe_move() == false) {
						moving = false;
						sprite->set_animation("stand_w");
					}
				}
			}
		}
		if (offset.y < 0) {
			offset.y += speed;
			if (offset.y >= 0) {
				offset.y = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else {
					if (following_path) {
						follow_path();
					}
					else if (maybe_move() == false) {
						moving = false;
						sprite->set_animation("stand_s");
					}
				}
			}
		}
		else if (offset.y > 0) {
			offset.y -= speed;
			if (offset.y <= 0) {
				offset.y = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else {
					if (following_path) {
						follow_path();
					}
					else if (maybe_move() == false) {
						moving = false;
						sprite->set_animation("stand_n");
					}
				}
			}
		}
	}

	return true;
}

void Map_Entity::draw(Point<float> draw_pos, bool use_depth_buffer)
{
	int add = moving ? -((int)((SDL_GetTicks() / 100) % 2) * bounce) : 0;

	// Draw each row separately for z-buffering
	sprite->get_current_image()->draw_z_single(
		Point<float>(draw_pos.x, draw_pos.y),
		// We multiply by 0.01f so the map transition which is 3D keeps graphics on the same plane.
		// 0.01f is big enough that a 16 bit depth buffer still works and small enough it looks right

		use_depth_buffer ? -(1.0f-((float)((position.y*noo.tile_size)+(offset.y*noo.tile_size)+z_add)/(float)(noo.map->get_tilemap()->get_size().h*noo.tile_size))) * 0.01f : 0.0f
	);
}

void Map_Entity::stop_now()
{
	if (following_path) {
		end_a_star();
	}
	stop_next_tile = false;
	moving = false;
	brain->l = brain->r = brain->u = brain->d = brain->b1 = false;
	set_direction(direction);
	sprite->reset();
	if (brain) {
		brain->reset();
	}
}

void Map_Entity::follow_path()
{
	if (path.size() == 0) {
		if (path_callback) {
			path_callback(path_callback_data);
			path_callback = 0;
		}
		end_a_star();
		stop_now();
		return;
	}
	A_Star::Node *node = path.front();
	path.pop_front();
	int dx = node->position.x - position.x;
	int dy = node->position.y - position.y;
	if (abs(dx)+abs(dy) != 1) {
		end_a_star();
		stop_now();
		return;
	}

	brain->l = brain->r = brain->u = brain->d = false;

	if (dx < 0) {
		brain->l = true;
	}
	else if (dx > 0) {
		brain->r = true;
	}
	else if (dy < 0) {
		brain->u = true;
	}
	else {
		brain->d = true;
	}

	moving = true;
	sitting = false;

	maybe_move();
}

void Map_Entity::end_a_star()
{
	following_path = false;
	path.clear();
}


bool Map_Entity::save(SDL_RWops *file)
{
	if (brain) {
		if (brain->save(file) == false) {
			return false;
		}
	}
	else {
		SDL_fprintf(file, "brain=0\n");
	}

	SDL_fprintf(file, "%s=", name.c_str());

	if (sprite) {
		std::string xml_filename;
		std::string image_directory;
		sprite->get_filenames(xml_filename, image_directory);
		std::string animation = sprite->get_animation();
		int started = sprite->is_started() ? 1 : 0;
		SDL_fprintf(file, "sprite=%s:%s:%s:%d", xml_filename.c_str(), image_directory.c_str(), animation.c_str(), started);
	}

	SDL_fprintf(file, ",position=%d:%d", position.x, position.y);

	SDL_fprintf(file, ",direction=%d", (int)direction);

	if (sitting) {
		SDL_fprintf(file, ",sitting=%d", (int)sitting);
	}

	if (z_add != 0) {
		SDL_fprintf(file, ",z_add=%d", z_add);
	}

	SDL_fprintf(file, "\n");

	return true;
}