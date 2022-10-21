#ifndef CHIP8_H
#define CHIP8_H

#include "memory.h"
#include "registers.h"
#include "stack.h"
#include "keyboard.h"
#include "screen.h"
#include <stddef.h>
struct chip8{
    struct memory mem;
    struct registers reg;
    struct stack stack;
    struct keyboard keyboard;
    struct screen screen;
};

void init(struct chip8* chip8);
void exec(struct chip8* chip8, unsigned short opcode);
void load(struct chip8* chip8, const char* buffer, size_t size);
#endif