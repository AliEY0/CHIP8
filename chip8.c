#include <stdio.h>
#include "chip8.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


uint8_t rand8(){
    srand(time(NULL));
    uint8_t num = rand() % 256;
  
    

    return num;
}
void setupGraphics(){
}
void setupInput(){

}

void clearScreen(Chip8 *chip8){
    for(int i = 0; i < 64 * 32; i++){
        chip8->screen[i] = 0;
    }
}

void initialize(Chip8 *chip8){
    chip8->I = 0;
    chip8->pc = 0x200;   // Startadres voor ROM
    chip8->sp = 0;       // Stack pointer op 0
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    
    memset(chip8->memory, 0, sizeof(chip8->memory));   
    memset(chip8->registers, 0, sizeof(chip8->registers));
    memset(chip8->stack, 0, sizeof(chip8->stack));
    memset(chip8->screen, 0, sizeof(chip8->screen));
    memset(chip8->keys, 0, sizeof(chip8->keys));

    
    for (int i = 0; i < 80; i++) {
        chip8->memory[0x50 + i] = font[i];
    }


}
void drawSprite(Chip8* chip8, uint8_t x, uint8_t y, uint8_t height){
    //https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#dxyn-display
    chip8->registers[0xF] = 0;
    for(uint8_t i = 0; i <  height; i++){
        uint8_t curRow = chip8->memory[chip8->I + i];
        for(int j = 0; j < 8; j++){
            uint8_t pixel = (curRow >> (7-j)) & 0x1; //Eerst van MSB beginnen
            uint8_t xPos = (x + j)%64;
            uint8_t yPos = (y + i)%32;
            if(pixel){
                if(chip8->screen[yPos*64 + xPos]){
                    chip8->registers[0xF] = 1;
                }
                chip8->screen[yPos*64 + xPos] ^= 1;
            } 
        }
    }
    chip8->drawFlag = true;
}
void emulateCycle(Chip8 *chip8){
    
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) |   (chip8->memory[chip8->pc+1]);
    chip8->pc += 2; 
    uint8_t nibble1 = opcode >> 12;
    uint8_t nibble2 = ((opcode >> 8)&0x000F);
    uint8_t nibble3 = ((opcode >> 4)&0x000F);
    uint8_t nibble4 = ((opcode)&0x000F);

    uint8_t nibble34 = ((opcode)&0x00FF);
    
    uint16_t nibble234 = ((opcode)&0x0FFF);


    if(nibble1 == 0){
        switch (opcode) {
            case 0x00E0:
                clearScreen(chip8);
                break;
            case 0x00EE:
                chip8->sp = chip8->sp - 1;
                chip8->pc = chip8->stack[chip8->sp];           
                break;
        }
    } 
    else if(nibble1 == 1){
        chip8->pc = nibble234;        
    }
    else if(nibble1 == 2){
        if(chip8->sp < 16){
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp += 1;
            chip8->pc = nibble234;     
        }
    }
    else if(nibble1 == 3){
        if(chip8->registers[nibble2] == nibble34){
            chip8->pc += 2;        
        }
    }
    else if(nibble1 == 4){
        if(chip8->registers[nibble2] != nibble34){
            chip8->pc += 2;        
        }
    }
    else if (nibble1 == 5){
        if(chip8->registers[nibble2] == chip8->registers[nibble3] ){
            chip8->pc += 2;        
        }
    }
    else if(nibble1 == 6){
        chip8->registers[nibble2] = nibble34;
    }

    else if(nibble1 == 7){
        chip8->registers[nibble2] += nibble34;
    }
    else if(nibble1 == 8){
       switch (nibble4) {
           case 0:
               chip8->registers[nibble2] = chip8->registers[nibble3];
               break;
           case 1:
               chip8->registers[nibble2] |= chip8->registers[nibble3];
               break; 
           case 2:
               chip8->registers[nibble2] &= chip8->registers[nibble3];
               break;
           case 3:
               chip8->registers[nibble2] ^= chip8->registers[nibble3];
               break;
           case 4:{
                   uint16_t sum = chip8->registers[nibble2] + chip8->registers[nibble3];
                   if(sum > 255){
                       chip8->registers[0xF] = 1;
                   } else{
                       chip8->registers[0xF] = 0;
                   }
                   chip8->registers[nibble2] = (sum & 0x00FF);
                   break;
               }
           case 5:
               
                
               if(chip8->registers[nibble2] >=  chip8->registers[nibble3]){
                    chip8->registers[0xF] = 1;
               } else {
                    chip8->registers[0xF] = 0;
               }
               chip8->registers[nibble2] -= chip8->registers[nibble3];
               break;
           case 6:
               
               chip8->registers[0xF] = (chip8->registers[nibble2] & 0x1);
               chip8->registers[nibble2] >>= 1;
               break;
           case 7:
               if(chip8->registers[nibble3] >=  chip8->registers[nibble2]){
                    chip8->registers[0xF] = 1;
               } else {
                    chip8->registers[0xF] = 0;
               }
               
               chip8->registers[nibble2] = chip8->registers[nibble3] - chip8->registers[nibble2];
               
               break;
           case 0xE:
               
                if(chip8->registers[nibble2] & 0x80){
                    chip8->registers[0xF] = 1;   
                } else{
                    chip8->registers[0xF] = 0;   
                }
               chip8->registers[nibble2] <<= 1;
               break;
       }

    }
    else if(nibble1 == 9){
        if(chip8->registers[nibble2] != chip8->registers[nibble3]){
             chip8->pc += 2;
        }
    }
    else if(nibble1 == 0xA){
        chip8->I = nibble234;

    }
    else if (nibble1 == 0xB){
        chip8->pc = chip8->registers[0] + nibble234;
    }
    else if(nibble1 == 0xC){
        chip8->registers[nibble2] = rand8() & nibble34;
    }
    else if(nibble1 == 0xD){ ////TO BE CONTINUED
        drawSprite(chip8, nibble2, nibble3, nibble4);
    }
    else if(nibble1 == 0xE){ ////TO BE CONTINUED
        switch (nibble34) {
            case 0x9E:

                break;
            case 0xA1:
                break;
        }
    }
    else if(nibble1 == 0XF){ //// TO BE CONTINUED
        switch (nibble34) {
            case 0x07:
                break;
            case 0x0A:
                break;
            case 0x15:
                break;
            case 0x18:
                break;
            case 0x1E:
                break;
            case 0x29:
                break;
            case 0x33:
                break;
            case 0x55:
                break;
            case 0x65:
                break;
        }
    }




















































}









































void loadGame(Chip8 *chip8, char *filename){
}
int main(int argc, char **argv) {
    Chip8 chip8;
    
    setupGraphics();
    setupInput(); 

    chip8.initialize(chip8);
    //chip8.loadGame(    


    return 0;
}


