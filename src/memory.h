#ifndef MEMORY_H
#define MEMORY_H

struct memory{
    unsigned char memory_array[4096];
};

void memory_set(struct memory* mem, int index, unsigned char value);
unsigned char memory_get(struct memory* mem, int index);
unsigned short memory_get_short(struct memory* mem, int index);

#endif 