#include "keyboard.h"
#include <assert.h>

void key_out_of_bound(int key){
    assert(key >= 0 && key < KEY_NUM);
}
// map is an array of virtual keys that will be used by chip8, key is the actual key we press down on the physical keyboard
int key_map(struct keyboard* board, char key){
    for(int i = 0; i < 16; i++){
        if(board->virtual_keys[i] == key){
            return i;
        }
    }
    return -1;
}
void keyboard_set_map(struct keyboard* board, const char* map){
    board->virtual_keys = map;
}

void key_down(struct keyboard* board, int key){
    board->key_array[key] = true;
}
void key_up(struct keyboard* board, int key){
    board->key_array[key] = false;
}
bool is_key_down(struct keyboard* board, int key){
    return board->key_array[key];
}