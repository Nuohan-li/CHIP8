#ifndef STACK_H
#define STACK_H

struct chip8;
struct stack{
    unsigned short stack_array[16];
};

void push(struct chip8* chip8, unsigned short value);
unsigned short pop(struct chip8* chip8);
#endif