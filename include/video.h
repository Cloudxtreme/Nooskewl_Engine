#ifndef VIDEO_H
#define VIDEO_H

void flip();

bool init_video();
void shutdown_video();

extern int screen_w;
extern int screen_h;

#endif // VIDEO_H