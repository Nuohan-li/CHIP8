#include <assert.h>
#include "memory.h"
#include "config.h"

static void out_of_bound(int index){
    assert(index >= 0 && index < MEMORY_SIZE);
}

void memory_set(struct memory* mem, int index, unsigned char value){
    out_of_bound(index);
    mem->memory_array[index] = value;
}
unsigned char memory_get(struct memory* mem, int index){
    out_of_bound(index);
    return mem->memory_array[index];
}
unsigned short memory_get_short(struct memory* mem, int index){
    unsigned char byte1 = memory_get(mem, index);
    unsigned char byte2 = memory_get(mem, index + 1);
    return byte1 << 8 | byte2;
}