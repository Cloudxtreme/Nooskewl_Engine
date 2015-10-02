#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/tilemap.h"
#include "Nooskewl_Engine/xml.h"

using namespace Nooskewl_Engine;

static void make_solid_callback(void *data)
{
	Map_Entity *entity = (Map_Entity *)data;

	entity->set_solid(true);
}

static int current_id;

void Map_Entity::new_game_started()
{
	current_id = 0;
}

Map_Entity::Map_Entity(std::string name) :
	name(name),
	direction(S),
	sprite(0),
	brain(0),
	position(0, 0),
	moving(false),
	speed(0.09f),
	offset(0.0f, 0.0f),
	solid(true),
	size(noo.tile_size, noo.tile_size),
	stop_next_tile(false),
	activate_next_tile(false),
	sitting(false),
	input_enabled(true),
	following_path(false),
	path_callback(0),
	shadow_type(SHADOW_NONE),
	has_blink(false),
	type(OTHER),
	stats(0),
	low(false),
	high(false),
	z(0),
	z_add(0),
	pre_sit_direction(DIRECTION_UNKNOWN),
	sit_directions(0),
	sat(false)
{
	id = current_id++;

	XML *xml = noo.miscellaneous_xml->find("blink");
	if (xml) {
		xml = xml->find(name);
		if (xml) {
			XML *eye = xml->find("eye");
			XML *blink = xml->find("blink");
			if (eye && blink) {
				XML *eye_r = eye->find("r");
				XML *eye_g = eye->find("g");
				XML *eye_b = eye->find("b");
				XML *blink_r = blink->find("r");
				XML *blink_g = blink->find("g");
				XML *blink_b = blink->find("b");
				if (eye_r && eye_g && eye_b && blink_r && blink_g && blink_b) {
					has_blink = true;
					eye_colour[0] = atoi(eye_r->get_value().c_str()) / 255.0f;
					eye_colour[1] = atoi(eye_g->get_value().c_str()) / 255.0f;
					eye_colour[2] = atoi(eye_b->get_value().c_str()) / 255.0f;
					eye_colour[3] = 1.0f;
					blink_colour[0] = atoi(blink_r->get_value().c_str()) / 255.0f;
					blink_colour[1] = atoi(blink_g->get_value().c_str()) / 255.0f;
					blink_colour[2] = atoi(blink_b->get_value().c_str()) / 255.0f;
					blink_colour[3] = 1.0f;
				}
			}
		}
	}

	set_next_blink();
}

Map_Entity::~Map_Entity()
{
	delete brain;
	delete sprite;
	delete stats;
}

void Map_Entity::set_brain(Brain *brain)
{
	this->brain = brain;
	if (brain) {
		brain->set_map_entity(this);
		// Update the brain one time so there are no potential flashes of state
		brain->update();
	}
}

void Map_Entity::load_sprite(std::string name)
{
	delete sprite;
	sprite = new Sprite(name);
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

void Map_Entity::set_size(Size<int> size)
{
	this->size = size;
}

void Map_Entity::set_offset(Point<float> offset)
{
	this->offset = offset;
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
	Point<int> stand_position;

	if (sitting == false) {
		if (pre_sit_direction != DIRECTION_UNKNOWN) {
			stand_position = position;

			if (direction == N) {
				stand_position.y--;
			}
			else if (direction == E) {
				stand_position.x++;
			}
			else if (direction == S) {
				stand_position.y++;
			}
			else {
				stand_position.x--;
			}
			if (noo.map->is_solid(-1, 0, stand_position, Size<int>(1, 1), true, true)) {
				return;
			}
		}
	}

	this->sitting = sitting;
	if (sitting) {
		moving = false;
	}
	set_direction(direction);

	if (sitting == false) {
		if (sat && direction == N) {
			set_z_add(get_z_add() + 1);
		}
		if (pre_sit_direction != DIRECTION_UNKNOWN) {
			std::list<A_Star::Node *> path = noo.map->find_path(position, stand_position);
			if (path.size() > 0) {
				pre_sit_direction = DIRECTION_UNKNOWN;
				set_path(path, make_solid_callback, this);
			}
		}
		sat = false;
	}
	else {
		if (direction == N) {
			set_z_add(get_z_add() - 1);
		}
		sat = true;
	}

	sit_directions = 0;
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

void Map_Entity::set_shadow_type(Shadow_Type shadow_type)
{
	this->shadow_type = shadow_type;
}

void Map_Entity::set_input_enabled(bool enabled)
{
	input_enabled = enabled;
}

void Map_Entity::set_type(Type type)
{
	this->type = type;
}

void Map_Entity::load_stats(std::string name)
{
	delete stats;
	stats = new Stats(name);
}

void Map_Entity::set_stats(Stats *stats)
{
	delete this->stats;
	this->stats = stats;
}

void Map_Entity::set_low(bool low)
{
	this->low = low;
}

void Map_Entity::set_high(bool high)
{
	this->high = high;
}

void Map_Entity::set_z(int z)
{
	this->z = z;
}

void Map_Entity::set_z_add(int z_add)
{
	this->z_add = z_add;
}

void Map_Entity::set_pre_sit_direction(Direction direction)
{
	this->pre_sit_direction = direction;
}

void Map_Entity::set_sit_directions(int sit_directions)
{
	this->sit_directions = sit_directions;
}

void Map_Entity::set_activate_next_tile(bool onoff)
{
	this->activate_next_tile = onoff;
}

void Map_Entity::set_name(std::string name)
{
	this->name = name;
}

void Map_Entity::set_stop_next_tile(bool stop_next_tile)
{
	this->stop_next_tile = stop_next_tile;
}

void Map_Entity::set_speed(float speed)
{
	this->speed = speed;
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
		return Point<float>(position.x*noo.tile_size+(offset.x*(float)noo.tile_size)+(noo.tile_size-sprite->get_current_image()->size.w)/2, (position.y+1)*noo.tile_size+(offset.y*(float)noo.tile_size)-h-z);
}

bool Map_Entity::is_solid()
{
	return solid;
}

bool Map_Entity::is_sitting()
{
	return sitting;
}

Map_Entity::Shadow_Type Map_Entity::get_shadow_type()
{
	return shadow_type;
}

bool Map_Entity::is_following_path()
{
	return following_path;
}

bool Map_Entity::is_input_enabled()
{
	return input_enabled;
}

Map_Entity::Type Map_Entity::get_type()
{
	return type;
}

Stats *Map_Entity::get_stats()
{
	return stats;
}

bool Map_Entity::is_low()
{
	return low;
}

bool Map_Entity::is_high()
{
	return high;
}

int Map_Entity::get_z()
{
	return z;
}

int Map_Entity::get_z_add()
{
	return z_add;
}

bool Map_Entity::is_moving()
{
	return moving;
}

float Map_Entity::get_speed()
{
	return speed;
}

bool Map_Entity::pixels_collide(Point<int> position, Size<int> size)
{
	Point<int> pos = this->position * noo.tile_size + this->offset * (float)noo.tile_size;
	pos.x += noo.tile_size / 2 - this->size.w / 2;
	pos.y -= (this->size.h - noo.tile_size);
	if (pos.x >= position.x+size.w || pos.x+this->size.w <= position.x || pos.y >= position.y+size.h || pos.y+this->size.h <= position.y) {
		return false;
	}
	return true;
}

bool Map_Entity::tiles_collide(Point<int> position, Size<int> size, Point<int> &collide_pos)
{
	Size<int> size2 = this->size / noo.tile_size;
	if (!(this->position.x >= position.x+size.w || this->position.x+size2.w <= position.x || this->position.y >= position.y+size.h || this->position.y+size2.h <= position.y)) {
		collide_pos = Point<int>(this->position.x-position.x, this->position.y-position.y);
		return true;
	}
	return false;
}

bool Map_Entity::entity_collides(Map_Entity *entity)
{
	Point<int> pos1 = (position + offset) * noo.tile_size;
	Size<int> size1 = size;
	pos1.x += noo.tile_size / 2 - size1.w / 2;
	pos1.y -= (size1.h - noo.tile_size);
	Point<int> pos2 = (entity->get_position() + entity->get_offset()) * noo.tile_size;
	Size<int> size2 = entity->get_size();
	pos2.x += noo.tile_size / 2 - size2.w / 2;
	pos2.y -= (size2.h - noo.tile_size);

	if ((pos1.x >= pos2.x+size2.w) || (pos1.x+size1.w <= pos2.x) || (pos1.y >= pos2.y+size2.h) || (pos1.y+size1.h <= pos2.y)) {
		return false;
	}

	return true;
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

	if ((following_path || input_enabled == true) && brain) {
		if (brain->l) {
			if (!sitting && (!solid || noo.map->is_solid(-1, this, position + Point<int>(-1, 0), Size<int>(1, 1)) == false)) {
				moving = true;
				offset = Point<float>(1, 0);
				position += Point<int>(-1, 0);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			if (!sitting || (sit_directions & Tilemap::Group::GROUP_CHAIR_WEST)) {
				if (sitting && direction == N) {
					set_z_add(get_z_add()+1);
				}
				set_direction(W);
			}
		}
		else if (brain->r) {
			if (!sitting && (!solid || noo.map->is_solid(-1, this, position + Point<int>(1, 0), Size<int>(1, 1)) == false)) {
				moving = true;
				direction = E;
				offset = Point<float>(-1, 0);
				position += Point<int>(1, 0);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			if (!sitting || (sit_directions & Tilemap::Group::GROUP_CHAIR_EAST)) {
				if (sitting && direction == N) {
					set_z_add(get_z_add()+1);
				}
				set_direction(E);
			}
		}
		else if (brain->u) {
			if (!sitting && (!solid || noo.map->is_solid(-1, this, position + Point<int>(0, -1), Size<int>(1, 1)) == false)) {
				moving = true;
				direction = N;
				offset = Point<float>(0, 1);
				position += Point<int>(0, -1);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			if (!sitting || (sit_directions & Tilemap::Group::GROUP_CHAIR_NORTH)) {
				if (sitting && direction != N) {
					set_z_add(get_z_add()-1);
				}
				set_direction(N);
			}
		}
		else if (brain->d) {
			if (!sitting && (!solid || noo.map->is_solid(-1, this, position + Point<int>(0, 1), Size<int>(1, 1)) == false)) {
				moving = true;
				direction = S;
				offset = Point<float>(0, -1);
				position += Point<int>(0, 1);
				ret = true;
			}
			else if (following_path) {
				stop_now();
			}
			if (!sitting || (sit_directions & Tilemap::Group::GROUP_CHAIR_SOUTH)) {
				if (sitting && direction == N) {
					set_z_add(get_z_add()+1);
				}
				set_direction(S);
			}
		}
		else if (brain->b1 && sitting) {
			set_sitting(false);
			set_direction(direction);
		}
	}

	return ret;
}

void Map_Entity::handle_event(TGUI_Event *event)
{
	if (brain) {
		brain->handle_event(event);
	}
}

bool Map_Entity::update(bool can_move)
{
	// Update stats
	if (stats) {
		int current_time = noo.get_play_time();
		if (stats->ate_time < current_time) {
			stats->ate_time = current_time;
			int step = MIN(20, 0xffff - stats->hunger);
			stats->hunger += step;
		}
		if (stats->drank_time < current_time) {
			stats->drank_time = current_time;
			int step = MIN(20, 0xffff - stats->thirst);
			stats->thirst += step;
		}
		if (stats->rested_time < current_time) {
			stats->rested_time = current_time;
			int step = MIN(20, stats->rest);
			stats->rest -= step;
		}
		if (stats->used_time < current_time) {
			stats->used_time = current_time;
			int step = MIN(20, 0xffff - stats->sobriety);
			stats->sobriety += step;
		}
		if (stats->status == Stats::SICK) {
			if (stats->status_start < current_time-(5*60)) {
				stats->set_status(Stats::NORMAL);
			}
		}
		if (stats->status == Stats::DRUNK) {
			if (stats->sobriety >= 0xffff * 3 / 4) {
				stats->set_status(Stats::NORMAL);
			}
		}
	}

	if (moving == false) {
		maybe_move();
	}

	float curr_speed;
	if (stats && stats->inventory && stats->inventory->get_total_weight() > stats->strength * 1000) {
		curr_speed = speed / 10;
	}
	else {
		curr_speed = speed;
	}

	if (moving) {
		if (offset.x < 0) {
			offset.x += curr_speed;
			if (offset.x >= 0) {
				offset.x = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
					if (!following_path && activate_next_tile) {
						activate_next_tile = false;
						noo.map->activate(this);
					}
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
			offset.x -= curr_speed;
			if (offset.x <= 0) {
				offset.x = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
					if (!following_path && activate_next_tile) {
						activate_next_tile = false;
						noo.map->activate(this);
					}
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
			offset.y += curr_speed;
			if (offset.y >= 0) {
				offset.y = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
					if (!following_path && activate_next_tile) {
						activate_next_tile = false;
						noo.map->activate(this);
					}
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
			offset.y -= curr_speed;
			if (offset.y <= 0) {
				offset.y = 0;
				noo.map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
					if (!following_path && activate_next_tile) {
						activate_next_tile = false;
						noo.map->activate(this);
					}
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

void Map_Entity::draw(Point<float> draw_pos, bool use_depth_buffer, bool sitting_n)
{
	Image *image = sprite->get_current_image();

	// We multiply by 0.01f so the map transition which is 3D keeps graphics on the same plane.
	// 0.01f is big enough that a 16 bit depth buffer still works and small enough it looks right
	float draw_z = use_depth_buffer ? -(1.0f - ((position.y*noo.tile_size)+(offset.y*noo.tile_size)+z_add)/(noo.map->get_tilemap()->get_size().h*noo.tile_size)) * 0.01f : 0.0f;

	Point<float> source_position(0.0f, 0.0f);
	Size<float> source_size((float)image->size.w, (float)image->size.h);

	if (sitting_n) {
		source_size.h *= 0.75f;
	}

	if (draw_pos.x < 0.0f) {
		source_position.x -= draw_pos.x;
		source_size.w += draw_pos.x;
		draw_pos.x = 0.0f;
	}
	if (draw_pos.x + source_size.w >= noo.screen_size.w) {
		float extra = (draw_pos.x + source_size.w) - noo.screen_size.w;
		source_size.w -= extra;
	}
	if (draw_pos.y < 0.0f) {
		source_position.y -= draw_pos.y;
		source_size.h += draw_pos.y;
		draw_pos.y = 0.0f;
	}
	if (draw_pos.y + source_size.h >= noo.screen_size.h) {
		float extra = (draw_pos.y + source_size.h) - noo.screen_size.h;
		source_size.h -= extra;
	}

	if (source_size.w > 0.0f && source_size.h > 0.0f) {
		if (has_blink) {
			noo.current_shader->set_bool("substitute_yellow", true);
			Uint32 ticks = SDL_GetTicks();
			if (ticks > next_blink) {
				if (ticks > next_blink + 50) {
					set_next_blink();
				}
				noo.current_shader->set_float_vector("substitute_colour", 4, blink_colour, 1);
			}
			else {
				noo.current_shader->set_float_vector("substitute_colour", 4, eye_colour, 1);
			}
		}

		image->draw_region_z_single(source_position, source_size, draw_pos, draw_z, 0);

		if (has_blink) {
			noo.current_shader->set_bool("substitute_yellow", false);
		}
	}
}

void Map_Entity::draw_shadows(Point<float> draw_pos)
{
	Image *image = sprite->get_current_image();

	if (shadow_type == SHADOW_TRANSLUCENT_COPY) {
		Shader *bak = noo.current_shader;
		noo.current_shader = noo.shadow_shader;
		noo.current_shader->use();
		noo.current_shader->set_float("alpha", 0.25f);

		image->draw_single(Point<float>(draw_pos.x, draw_pos.y+4));

		noo.current_shader = bak;
		noo.current_shader->use();
	}
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
	if (offset.x != 0 || offset.y != 0) {
		return;
	}

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

	maybe_move();
}

void Map_Entity::end_a_star()
{
	following_path = false;
	path.clear();
	if (activate_next_tile) {
		activate_next_tile = false;
		noo.map->activate(this);
	}
}


bool Map_Entity::save(std::string &out)
{
	if (brain) {
		if (brain->save(out) == false) {
			return false;
		}
	}
	else {
		out += string_printf("brain=0\n");
	}

	out += string_printf("%s=", name.c_str());

	if (sprite) {
		std::string xml_filename;
		std::string image_directory;
		sprite->get_filenames(xml_filename, image_directory);
		std::string animation = sprite->get_animation();
		int started = sprite->is_started() ? 1 : 0;
		out += string_printf("sprite=%s:%s:%s:%d", xml_filename.c_str(), image_directory.c_str(), animation.c_str(), started);
	}

	out += string_printf(",position=%d:%d", position.x, position.y);

	out += string_printf(",direction=%d", (int)direction);

	out += string_printf(",speed=%f", speed);

	if (sitting) {
		out += string_printf(",sitting=%d", (int)sitting);
		out += string_printf(",pre_sit_direction=%d", (int)pre_sit_direction);
	}

	if (z != 0) {
		out += string_printf(",z=%d", z);
	}

	if (shadow_type != SHADOW_NONE) {
		out += string_printf(",shadow_type=%d", (int)shadow_type);
	}

	if (solid == false) {
		out += string_printf(",solid=%d", solid ? 1 : 0);
	}

	if (low == true) {
		out += string_printf(",low=%d", low ? 1 : 0);
	}

	if (high == true) {
		out += string_printf(",high=%d", high ? 1 : 0);
	}

	if (z_add != 0) {
		int z = z_add;
		if (sitting && direction == N) {
			z += 1;
		}
		out += string_printf(",z_add=%d", z);
	}

	if (stats != 0) {
		out += string_printf(",stats", stats != 0 ? 1 : 0);
		if (stats->inventory != 0) {
			out += string_printf(",inventory", stats->inventory != 0 ? 1 : 0);
		}
	}

	out += string_printf("\n");

	if (stats != 0) {
		out += string_printf(
			"name=%s,profile_pic=%s,alignment=%d,sex=%d,hp=%d,max_hp=%d,mp=%d,max_mp=%d,attack=%d,defense=%d,agility=%d,luck=%d,speed=%d,strength=%d,experience=%d,karma=%d,hunger=%d,thirst=%d,rest=%d,sobriety=%d,weapon=%d,armour=%d,status=%d,status_start=%d\n",
			stats->name.c_str(),
			stats->profile_pic->filename.c_str(),
			(int)stats->alignment,
			(int)stats->sex,
			stats->hp,
			stats->max_hp,
			stats->mp,
			stats->max_mp,
			stats->attack,
			stats->defense,
			stats->agility,
			stats->luck,
			stats->speed,
			stats->strength,
			stats->experience,
			stats->karma,
			stats->hunger,
			stats->thirst,
			stats->rest,
			stats->sobriety,
			stats->weapon_index,
			stats->armour_index,
			(int)stats->status,
			stats->status_start
		);

		if (stats->inventory != 0) {
			out += stats->inventory->to_string();
		}
	}

	return true;
}

void Map_Entity::set_next_blink()
{
	next_blink = SDL_GetTicks() + 3000 + (rand() % 3000);
}
