#include "screen.h"
#include <assert.h>
#include <string.h>

void screen_out_of_bound(int x, int y){
    assert(x >= 0 && x < 64 && y >= 0 && y < 32 );
}

void screen_set(struct screen* screen, int x, int y){
    screen_out_of_bound(x, y);
    screen->pixels_array[y][x] = true;
}
bool is_screen_set(struct screen* screen, int x, int y){
    screen_out_of_bound(x, y);
    return screen->pixels_array[y][x];
}

bool draw_sprite(struct screen* screen, int x, int y, const char* sprite_ptr, int num_byte){
    bool sprite_collision  = false;
    for(int ly = 0; ly < num_byte; ly++){
        char c = sprite_ptr[ly];
        for(int lx = 0; lx < 8; lx++){
            // determining whether we should draw a pixel or not, suppose that we have 11011111, we AND this with 0b10000000, if the result
            // is bigger than 0, that means we have a 1, meaning that we need to draw a pixel, we then shift 1 from 0b10000000 to the left
            // which becomes 0b01000000, and we keep doing this operation. if we have 0 AND 1, we know that the result will be 0, so we don't
            // have to draw a pixel there, there is nothing to do in this case, so simply continue onto the next loop and skip the drawing part. 
            if((c & (0b10000000 >> lx)) == 0){
                continue;
            }
            // x and y is the position of the first pixel, we then draw other pixels depending on the result from above if statement
            // so if at position lx = 2, we got a non-zero result, we then have to draw pixel at x and x+2 and so on.
            // modulo used to make sure the sprite wraps around the screen, meaning that if we go off bound, then the other part of sprit
            // shows up at x or y = 0. e.g 
            if(screen->pixels_array[(ly+y) % 32][(lx+x) % 64]){
                sprite_collision  = true;
            }
            // we use XOR = true -> required from technical reference, if this causes any pixels to be erased Vf should be set to 1, 0 otherwise
            screen->pixels_array[(ly+y) % 32][(lx+x) % 64] ^= true;
        }
    }

    return sprite_collision;
}

void clear(struct screen* screen){
    memset(screen->pixels_array, 0, sizeof(screen->pixels_array));
}