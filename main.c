//https://github.com/Error916/Chip8_emulator/blob/master/src/main.c

#include <SDL2/SDL.h>
#include "chip8.h"
#include <stdint.h>
#include <stdio.h>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define SCALE 10
#define CLOCK_RATE_MS 2

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <ROM>\n", argv[0]);
        return 1;
    }

    const char *romFilename = argv[1];

    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        VIDEO_WIDTH * SCALE, VIDEO_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             VIDEO_WIDTH, VIDEO_HEIGHT);

    // Init CHIP-8
    Chip8 chip8 = {0};
    initialize(&chip8);
    loadGame(&chip8, (char *)romFilename);

    uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];
    int quit = 0;

    while (!quit) {
        // Process SDL input events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;

            int val = (event.type == SDL_KEYDOWN) ? 1 : 0;
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = 1; break;
                    case SDLK_1: chip8.keys[0x1] = val; break;
                    case SDLK_2: chip8.keys[0x2] = val; break;
                    case SDLK_3: chip8.keys[0x3] = val; break;
                    case SDLK_4: chip8.keys[0xC] = val; break;
                    case SDLK_q: chip8.keys[0x4] = val; break;
                    case SDLK_w: chip8.keys[0x5] = val; break;
                    case SDLK_e: chip8.keys[0x6] = val; break;
                    case SDLK_r: chip8.keys[0xD] = val; break;
                    case SDLK_a: chip8.keys[0x7] = val; break;
                    case SDLK_s: chip8.keys[0x8] = val; break;
                    case SDLK_d: chip8.keys[0x9] = val; break;
                    case SDLK_f: chip8.keys[0xE] = val; break;
                    case SDLK_z: chip8.keys[0xA] = val; break;
                    case SDLK_x: chip8.keys[0x0] = val; break;
                    case SDLK_c: chip8.keys[0xB] = val; break;
                    case SDLK_v: chip8.keys[0xF] = val; break;
                }
            }
        }

        // Emulate one CHIP-8 cycle
        emulateCycle(&chip8);

        // Update pixel buffer
        for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
            pixels[i] = chip8.screen[i] ? 0xFFFFFFFF : 0x00000000;
        }

        // Render
        SDL_UpdateTexture(texture, NULL, pixels, VIDEO_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        chip8.drawFlag = 0;
        SDL_Delay(CLOCK_RATE_MS);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

