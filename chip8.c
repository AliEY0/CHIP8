#include <stdio.h>
#include "chip8.h"
#include <stdint.h>
#include <stdbool.h>



void setupGraphics(){
}
void setupInput(){

}
void initialize(Chip8 *chip8){

}

void emulateCycle(Chip8 *chip8){
    
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) |   (chip8->memory[chip8->pc+1] << 8);
    chip8->pc += 2; 
    uint8_t nibble1 = opcode >> 12;
    uint8_t nibble2 = ((opcode >> 8)&0x000F);
    uint8_t nibble3 = ((opcode >> 4)&0x000F);
    uint8_t nibble4 = ((opcode)&0x000F);

    uint8_t nibble23 = ((opcode>> 4)&0x00FF);
    
    uint16_t nibble234 = ((opcode)&0x0FFF);




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


