#ifndef STACK_H
#define STACK_H
#include "config.h"

struct chip8;
struct stack{
    unsigned short stack_array[STACK_SIZE];
};

void push(struct chip8* chip8, unsigned short value);
unsigned short pop(struct chip8* chip8);
#endif