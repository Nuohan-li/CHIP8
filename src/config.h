#ifndef CONFIG_H
#define CONFIG_H

#define EMULATOR_TITLE "CHIP-8 EMULATOR"
// 10 times the size of the orginal display specification, making it 640 x 320
#define WINDOW_SCALE 10 
#define MEMORY_SIZE 4096
// display width and height
#define WIDTH 64
#define HEIGHT 32

#define REGISTER_NUM 16
// stack is an array of 16 16bit items 
#define STACK_SIZE 16
#define KEY_NUM 16

// The default character set should be stored in the interpreter area of Chip-8 memory (0x000 to 0x1FF) from reference
#define CHARACTER_SET_ADDRESS 0X000
#define PROGRAM_LOAD_ADDR 0X200
#endif