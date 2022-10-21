#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "keyboard.h"

// these are physical keyboard keys, their index is mapped to chip8 virtual keys e.g 0x00 at index 0 is mapped to 1 for chip8 key
const char virtual_keys[KEY_NUM] = {SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, 
                                SDLK_9, SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f};

int main(int argc, char **argv){

    // argc = argument counter 
    // argv = arguments we passed in when launching the program
    if(argc < 2){
        printf("You must provide a file to load");
        return -1;
    }
    const char* file_name = argv[1];
    printf("file name is: %s\n", file_name);
    // open file and set mode to read binary
    FILE* f = fopen(file_name, "rb");
    if(!f){
        printf("failed to open file");
        return -1;
    }
    // getting the size of buffer and read from file then save to file

    // fseek to the end of the file
    fseek(f, 0, SEEK_END);
    // asks operation for the current position, which is the end, and save that as our size
    long size = ftell(f);
    // move all the way back to the beginning where we will actually read the file
    fseek(f, 0, SEEK_SET);

    char buffer[size];
    int result = fread(buffer, size, 1, f);
    if(result != 1){
        printf("failed to read from file");
        return -1;
    }
    struct chip8 chip8;
    init(&chip8);
    load(&chip8, buffer, size);
    keyboard_set_map(&chip8.keyboard, virtual_keys);

    // initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    // creating the window
    SDL_Window* window = SDL_CreateWindow(EMULATOR_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * WINDOW_SCALE, HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    while(1){

        // handling quit event
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                SDL_DestroyWindow(window);
                return 0;
            }
            else if(event.type == SDL_KEYDOWN){
                // getting the key pressed
                char key = event.key.keysym.sym;
                int virtual_key = key_map(&chip8.keyboard, key);
                if(virtual_key != -1){
                    printf("%x\n", virtual_key);
                    key_down(&chip8.keyboard, virtual_key);
                }
            }
            else if(event.type == SDL_KEYUP){
                char key = event.key.keysym.sym;
                int virtual_key = key_map(&chip8.keyboard, key);
                if(virtual_key != -1){
                    key_up(&chip8.keyboard, virtual_key);
                    printf("%x\n", virtual_key);
                }   
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                
                if(is_screen_set(&chip8.screen, x, y)){
                    SDL_Rect r;
                    r.x = x * WINDOW_SCALE; // since we are scaling everything up by 10 times, so every 10 pixels are 1 pixel for the emulation
                    r.y = y * WINDOW_SCALE;
                    r.w = WINDOW_SCALE;
                    r.h = WINDOW_SCALE;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }
        SDL_RenderPresent(renderer);
        
        // delay timer, using sleep to simulate delaying by 60Hz
        if(chip8.reg.delay_timer > 0){
            Sleep(5);
            chip8.reg.delay_timer -= 1;
        }

        // simulating sound timer
        if(chip8.reg.sound_timer > 0){
            Beep(1500, 100);
            chip8.reg.sound_timer -= 1;
        }

        // reading two bytes from where the program counter is pointing to, which is the intruction
        unsigned short opcode = memory_get_short(&chip8.mem, chip8.reg.program_counter);
        chip8.reg.program_counter += 2;
        exec(&chip8, opcode);
    }    


    SDL_DestroyWindow(window);
    return 0;
}