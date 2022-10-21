#include "chip8.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SDL2/SDL.h"

// section 2.4 of the reference a binary representation can be found there as well
// if a bit = 1, then pixel there is on, otherwise it's off
const char default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
    0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
    0x90, 0x90, 0xf0, 0x10, 0x10, // 4
    0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
    0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
    0xf0, 0x10, 0x20, 0x40, 0x40, // 7
    0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
    0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
    0xf0, 0x90, 0xf0, 0x90, 0x90, // a
    0xe0, 0x90, 0xe0, 0x90, 0xe0, // b 
    0xf0, 0x80, 0x80, 0x80, 0xf0, // c
    0xe0, 0x90, 0x90, 0x90, 0xe0, // d
    0xf0, 0x80, 0xf0, 0x80, 0xf0, // e
    0xf0, 0x80, 0xf0, 0x80, 0x80  // f
};
void init(struct chip8* chip8){
    // set everything to 0
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->mem.memory_array, default_character_set, sizeof(default_character_set));
}
void load(struct chip8* chip8, const char* buffer, size_t size){
    // making sure we are not going out of bound -> program load area starts from 0x200
    assert(size + 0x200 < 4096);
    // loading the program into the memory, buffer is the source
    memcpy(&chip8->mem.memory_array[0x200], buffer, size);
    // have program counter point to the beginning of the intructions, which is 0x200
    chip8->reg.program_counter = 0x200;
}
char wait_for_key_press(struct chip8* chip8){
    SDL_Event event;
    while(SDL_WaitEvent(&event)){
        if(event.type != SDL_KEYDOWN){
            continue;
        }
        char c = event.key.keysym.sym;
        char key = key_map(&chip8->keyboard, c);
        if(key != -1){
            return key;
        }
    }
}
void exec_4(struct chip8* chip8, unsigned short opcode){
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned short n = opcode & 0x000f;
    switch(opcode & 0x00ff){
        // Fx07 - LD Vx, DT
        // Set Vx = delay timer value.
        // The value of DT is placed into Vx.
        case 0x0007:
            chip8->reg.V[x] = chip8->reg.delay_timer;
        break;
        // Fx0A - LD Vx, K
        // Wait for a key press, store the value of the key in Vx.
        // All execution stops until a key is pressed, then the value of that key is stored in Vx.
        case 0x000A:
            chip8->reg.V[x] = wait_for_key_press(chip8);
        break;
        // Fx15 - LD DT, Vx
        // Set delay timer = Vx.
        // DT is set equal to the value of Vx.
        case 0x0015:
            chip8->reg.delay_timer = chip8->reg.V[x];
        break;
        // Fx18 - LD ST, Vx
        // Set sound timer = Vx.
        // ST is set equal to the value of Vx.
        case 0x0018:
            chip8->reg.sound_timer = chip8->reg.V[x];
        break;
        // Fx1E - ADD I, Vx
        // Set I = I + Vx.
        // The values of I and Vx are added, and the results are stored in I.
        case 0x001E:
            chip8->reg.I += chip8->reg.V[x]; 
        break;
        // Fx29 - LD F, Vx
        // Set I = location of sprite for digit Vx.
        // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. 
        case 0x0029:
            // default height for sprites is 5
            chip8->reg.I = chip8->reg.V[x] * 5;
        break;
        // Fx33 - LD B, Vx
        // Store BCD representation of Vx in memory locations I, I+1, and I+2.
        // The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, 
        // the tens digit at location I+1, and the ones digit at location I+2.
        case 0x0033:
            // ex: 523 / 100 = 5.23 -> it is int here, so it becomes 5
            // ex: 523 / 10 = 52 (int), 52 % 10 = 2
            // ex: 523 % 10 = 3 ==> 52 * 10 + 3 = 523
            memory_set(&chip8->mem, chip8->reg.I, chip8->reg.V[x] / 100);
            memory_set(&chip8->mem, chip8->reg.I+1, chip8->reg.V[x] / 10 % 10);
            memory_set(&chip8->mem, chip8->reg.I+2, chip8->reg.V[x] % 10);
        break;
        // Fx55 - LD [I], Vx
        // Store registers V0 through Vx in memory starting at location I.
        // The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
        case 0x0055:
            for(int i = 0; i <= x; i++){
                memory_set(&chip8->mem, chip8->reg.I+i, chip8->reg.V[x]);
            }
        break;
        // Fx65 - LD Vx, [I]
        // Read registers V0 through Vx from memory starting at location I.
        // The interpreter reads values from memory starting at location I into registers V0 through Vx.
        case 0x0065:
            for(int i = 0; i <= x; i++){
                chip8->reg.V[i] = memory_get(&chip8->mem, chip8->reg.I+i); 
            }
        break;
    }
}
void exec_3(struct chip8* chip8, unsigned short opcode){
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned short tmp = 0;
    switch(opcode & 0x000f){
        // 8xy0 - LD Vx, Vy
        // Set Vx = Vy.
        // Stores the value of register Vy in register Vx.
        case 0x0000:
            chip8->reg.V[x] = chip8->reg.V[y];
        break;
        // 8xy1 - OR Vx, Vy
        // Set Vx = Vx OR Vy.
        // Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.  
        case 0x0001:
            chip8->reg.V[x] |= chip8->reg.V[y];
        break;
        // 8xy2 - AND Vx, Vy
        // Set Vx = Vx AND Vy.
        // Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
        case 0x0002:
            chip8->reg.V[x] &= chip8->reg.V[y];
        break;
        // 8xy3 - XOR Vx, Vy
        // Set Vx = Vx XOR Vy.
        // Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
        case 0x0003:
            chip8->reg.V[x] ^= chip8->reg.V[y];
        break;
        // 8xy4 - ADD Vx, Vy
        // Set Vx = Vx + Vy, set VF = carry.
        // The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) 
        // VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
        case 0x0004:
        // not sure why it is not working - carry flag won't be set 
            // chip8->reg.V[x] += chip8->reg.V[y];
            // if(chip8->reg.V[x] > 0xff){
            //     chip8->reg.V[15] = 1;
            // }
            // else{
            //     printf("here inside 0x0004 v[0]= %x\n ", chip8->reg.V[x]);
            //     chip8->reg.V[15] = 0;
            // }

            tmp = chip8->reg.V[x] + chip8->reg.V[y];
            if(tmp > 0xff){
                chip8->reg.V[15] = 1;
            }
            else{
                chip8->reg.V[15] = 0;
            }
            chip8->reg.V[x] = tmp;
        break;
        // 8xy5 - SUB Vx, Vy
        // Set Vx = Vx - Vy, set VF = NOT borrow.
        // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
        case 0x0005:
            tmp = chip8->reg.V[x] - chip8->reg.V[y];
            if(chip8->reg.V[x] > chip8->reg.V[y]){
                chip8->reg.V[15] = 1;
            }
            else{
                chip8->reg.V[15] = 0;
            }
            chip8->reg.V[x] = tmp;
        break;
        // 8xy6 - SHR Vx {, Vy}
        // Set Vx = Vx SHR 1.
        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
        case 0x0006:
            tmp = chip8->reg.V[x] / 2;
            if(chip8->reg.V[x] & 0x01 == 1){
                chip8->reg.V[15] = 1;
            }
            else{
                chip8->reg.V[15] = 0;
            }
            chip8->reg.V[x] = tmp;
        break;
        // 8xy7 - SUBN Vx, Vy
        // Set Vx = Vy - Vx, set VF = NOT borrow.
        // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
        case 0x0007:
            tmp = chip8->reg.V[y] - chip8->reg.V[x];
            if(chip8->reg.V[y] > chip8->reg.V[x]){
                chip8->reg.V[15] = 1;
            }
            else{
                chip8->reg.V[15] = 0;
            }
            chip8->reg.V[x] = tmp;
        break;
        // 8xyE - SHL Vx {, Vy}
        // Set Vx = Vx SHL 1.
        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        case 0x000E:
            tmp = chip8->reg.V[x] * 2;
            if(chip8->reg.V[x] & 0x80 == 1){
                chip8->reg.V[15] = 1;
            }
            else{
                chip8->reg.V[15] = 0;
            }
            chip8->reg.V[x] = tmp;
        break;
    }
}

void exec_2(struct chip8* chip8, unsigned short opcode){
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned short n = opcode & 0x000f;

    switch (opcode & 0xf000){
        // 1nnn - JP addr
        // Jump to location nnn.
        // The interpreter sets the program counter to nnn.
        case 0x1000:
            chip8->reg.program_counter = nnn;
        break;
        // 2nnn - CALL addr
        // Call subroutine at nnn. 
        // The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
        case 0x2000:
            push(chip8, chip8->reg.program_counter);
            chip8->reg.program_counter = nnn;
        break;
        // 3xkk - SE Vx, byte
        // Skip next instruction if Vx = kk, 
        // The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
        case 0x3000:
            if(chip8->reg.V[x] == kk){
                // += 2 because each intruction is 2 bytes
                chip8->reg.program_counter += 2;
            }
        break;
        // 4xkk - SNE Vx, byte
        // Skip next instruction if Vx != kk.
        // The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
        case 0x4000:
            if(chip8->reg.V[x] != kk){
                chip8->reg.program_counter += 2;
            }
        break;
        // 5xy0 - SE Vx, Vy
        // Skip next instruction if Vx = Vy.
        // The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
        case 0x5000:
            if(chip8->reg.V[x] == chip8->reg.V[y]){
                chip8->reg.program_counter += 2;
            }
        break;
        // 6xkk - LD Vx, byte
        // Set Vx = kk.
        // The interpreter puts the value kk into register Vx.
        case 0x6000:
            chip8->reg.V[x] = kk;
        break;
        // 7xkk - ADD Vx, byte
        // Set Vx = Vx + kk.
        // Adds the value kk to the value of register Vx, then stores the result in Vx.
        case 0x7000:
            chip8->reg.V[x] += kk;
        break; 
        // 8xy0 - LD Vx, Vy
        // Set Vx = Vy.
        // Stores the value of register Vy in register Vx.
        case 0x8000:
            exec_3(chip8, opcode);
        break;
        // 9xy0 - SNE Vx, Vy
        // Skip next instruction if Vx != Vy.
        // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
        case 0x9000:
            if(chip8->reg.V[x] != chip8->reg.V[y]){
                chip8->reg.program_counter += 2;
            }
        break;
        // Annn - LD I, addr
        // Set I = nnn.
        // The value of register I is set to nnn.
        case 0xA000:
            chip8->reg.I = nnn;
        break;
        // Bnnn - JP V0, addr
        // Jump to location nnn + V0.
        // The program counter is set to nnn plus the value of V0.
        case 0xB000:
            chip8->reg.program_counter = nnn + chip8->reg.V[0];
        break;
        // Cxkk - RND Vx, byte
        // Set Vx = random byte AND kk.
        // The interpreter generates a random number from 0 to 255, 
        // which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
        case 0xC000:
            srand(clock());
            chip8->reg.V[x] = (rand() % 255) & kk;
        break;
        // Dxyn - DRW Vx, Vy, nibble
        // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        // The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at
        // coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, 
        // otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, 
        // it wraps around to the opposite side of the screen. 
        // See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.

        // n = height of sprite
        case 0xD000: 
            // draw sprite function returns true is there is collision, thus setting V[15] to 1
            chip8->reg.V[15] = draw_sprite(&chip8->screen, chip8->reg.V[x], chip8->reg.V[y], (const char*) &chip8->mem.memory_array[chip8->reg.I], n);
        break;
        // keyboard operation
        case 0xE000:
        {   
            // Ex9E - SKP Vx
            // Skip next instruction if key with the value of Vx is pressed.
            // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
            if(opcode & 0x00ff == 0x009e){
                if(is_key_down(&chip8->keyboard, chip8->reg.V[x])){
                    chip8->reg.program_counter += 2;
                }
            }
            // ExA1 - SKNP Vx
            // Skip next instruction if key with the value of Vx is not pressed.
            // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
            if(opcode & 0x00ff == 0x00A1){
                if(!is_key_down(&chip8->keyboard, chip8->reg.V[x])){
                    chip8->reg.program_counter += 2;
                }
            }
        }
        break;

        case 0xF000:
            exec_4(chip8, opcode);
        break;
    }
}
// implementation of opcodes
void exec(struct chip8* chip8, unsigned short opcode){
    switch(opcode){
        // 00E0 - CLS
        // clear the screen
        case 0x00E0:
            clear(&chip8->screen);
        break;
        // 00EE - RET
        // The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
        case 0x00EE:
            chip8->reg.program_counter = pop(chip8);
        break;

        default: 
            exec_2(chip8, opcode);
    }
}