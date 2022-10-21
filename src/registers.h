#ifndef REGISTERS_H
#define REGISTERS_H

#include "config.h"
struct registers{
    // one char in C is 1 byte = 8 bit 
    unsigned char V[REGISTER_NUM];
    // special 16 bit register in chip-8, used to store memory address
    unsigned short I;
    // see section 2.5 reference
    unsigned char delay_timer;
    // see section 2.5 reference
    unsigned char sound_timer;
    // 16bit, points to the next instruction that will be run after the current one
    unsigned short program_counter;
    // 8 bit, points to the topmost level of the stack
    unsigned char stack_pointer;
};

#endif