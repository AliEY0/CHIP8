#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct{
  uint8_t memory[4096];
  uint8_t registers[16]; //V0 .. VF
  uint16_t I; //Adress Register
  uint16_t stack[16];
  uint8_t sound_timer;
  uint8_t delay_timer;
  uint8_t keys[16];
  uint8_t screen[64*32];
  uint16_t pc;
} Chip8;


#endif
