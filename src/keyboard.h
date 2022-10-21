#ifndef KEYBOARD_H
#define KEYBOARD_H

// section 2.3 chip8 reference 
#include "config.h"
#include <stdbool.h>
struct keyboard{
    bool key_array[KEY_NUM];
    char* virtual_keys;
};

void keyboard_set_map(struct keyboard* board, const char* map);
// mapping real keys on keyboard to chip 8 virtual key
int key_map(struct keyboard* board, char key);
// functions expect the virtual key
void key_down(struct keyboard* board, int key);
void key_up(struct keyboard* board, int key);
bool is_key_down(struct keyboard* board, int key);

#endif