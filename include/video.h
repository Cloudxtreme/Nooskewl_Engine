#ifndef VIDEO_H
#define VIDEO_H

void flip();

void init_video();
void shutdown_video();

void set_default_projection();
void set_map_transition_projection(float angle);

extern int screen_w;
extern int screen_h;

#endif // VIDEO_H