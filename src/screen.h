#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

// a 2d array representing the 
struct screen{
    bool pixels_array[32][64];
};

void clear(struct screen* screen);
void screen_set(struct screen* screen, int x, int y);
bool is_screen_set(struct screen* screen, int x, int y);
bool draw_sprite(struct screen* screen, int x, int y, const char* sprite_ptr, int num_byte);
#endif