#include "stack.h"
#include "chip8.h"
#include <assert.h>

static void out_of_bound(struct chip8* chip8){
    assert(chip8->reg.stack_pointer < 16);
}

void push(struct chip8* chip8, unsigned short value){
    out_of_bound(chip8);
    // stack pointer points to the topmost level of the stack, so we push there
    chip8->stack.stack_array[chip8->reg.stack_pointer] = value;
    // incrementing the stack pointer by 1 so it is kept at the topmost level
    chip8->reg.stack_pointer += 1;
}

unsigned short pop(struct chip8* chip8){
    chip8->reg.stack_pointer -= 1;
    out_of_bound(chip8);
    return chip8->stack.stack_array[chip8->reg.stack_pointer];
}