#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

int initialize(){

  /* clear all registers and memory */

  memset(memory, 0, 4096);
  memset(V,0,16);
  I = 0;
  PC = 0x200; // program counter starts from byte 512.
  SP = -1;
  sound = 0;
  delay = 0;
  memset(stack, 0, 32);
  rom_end_addr = 0;
  // load font data into memory.
  /*
  unsigned char letters[16*5] = {
    0xF0,0x90,0x90,0x90,0xF0,
    0x20,0x60,0x20,0x20,0x70
  }
  */

  printf("%s\n", "Initialized Values");
  return 0;
}

int load_rom(char* rom_path)
{

  // rom should be loaded from 0x200
  // read the file byte by byte, and put instructions into memory starting from 0x200.
  FILE* rom_file = fopen(rom_path, "r");
  if(rom_file == NULL)
  {
    return -1;
  }

  char buffer[256] = {0};

  unsigned short m_index = 0x200;
  unsigned short read_size = 0;
  while( (read_size = fread((void*)buffer, 1, 256, rom_file)) > 0)
  {
    for(int i=0;i<read_size && m_index < 4096;i++,m_index++)
    {
      memory[m_index] = buffer[i];
    }
  }
  rom_end_addr = m_index -1;
  printf("%s %s\n", rom_path, "loaded");
  return 0;
}

int emulate_cycle(){
  // each instruction is 16 bits long, 2 bytes.
  if(PC < rom_end_addr){
  // instructions are given MSB, most significant byte first.
  unsigned short opcode = ((memory[PC] << 8) | memory[PC+1]);
  printf("%X , ", opcode);

  unsigned short opcode_type = opcode  & 0xF000;
  printf("Type: %X\n", opcode_type);

  switch(opcode_type)
  {

    case 0x0000:
    {

      switch(opcode & 0x00FF)
      {
        case 0x00E0:
        printf("clearing the display\n");
          // clear the display
        break;

        case 0x00EE:
        printf("return from subroutine, set PC = %X\n", stack[SP]);
          // return from subroutine
          // set PC to addr at top of stack.
         // PC = stack[SP];
         // stack[SP] = 0; // clear out stack entry.
         // SP--; // decrement stack pointer

        break;

        default:
          instruction_error(opcode);
          break;

      }

      break;
    }

    case 0x1000:
    {
      // jump to location nnn
      unsigned short jump_address = (opcode & 0x0FFF);
    //  PC = jump_address;
      printf("jumped to address %X in memory\n", jump_address);
      break;
    }

    case 0x2000:
    {
      // call subroutine at address nnn
      unsigned short subroutine_address = opcode & 0x0FFF;
      SP++; // new spot on the stack.
      stack[SP] = PC;
      PC = subroutine_address;
      printf("called subroutine at address %X in memory\n", subroutine_address);
    break;
  }

    case 0x3000:
    {
      // 3xkk
      // skip next instruction if Vx = kk
      printf("Skipping next instruction\n");
      if(V[(opcode & 0x0F00 >> 8)] == (opcode & 0x00FF)){
        PC += 4; // each instruction is 2 bytes.
        return 0;
      }
    break;
    }

    case 0x4000:
      // 4xkk
      // if Vx != kk skip the next instruction
     printf("Skipping next instruction\n");
      if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
      {
        PC += 4;
        return 0;
      }
    break;

    case 0x5000:
      // 5xy0
      // if vx and xy are equal, skip the next instruction
     printf("Skipping next instruction\n");
      if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
      {
        PC += 4;
        return 0;
      }
    break;

    case 0x6000:
      /*Set Vx = kk.*/
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
      printf("Load register: %u with %u\n", (opcode & 0x0F00) >> 8, opcode & 0x00FF); 
      break;

    case 0x7000:
      //7xkk 
      // adds kk to register Vx and stores it there.
      printf("Adding %u to register: %u\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
      V[(opcode & 0x0F00) >> 8]+= (opcode & 0x00FF);

    break;

    case 0x8000:
    {
      // 8xyn
      printf("Math operations with Registers: %u, %u\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
      switch(opcode & 0x000F){

        case 0x0001:
          // set Vx = Vx | Vy.
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];

        break;
        case 0x0002:
          // set Vx = Vx & Vy
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];

        break;
        case 0x0003:
          // set Vx = Vx ^ Vy
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];


        break;
        case 0x0004:
        {
          // set Vx = Vx + Vy, set VF = carry.
          unsigned short sum = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
         // if there was a carry, then the 9th bit would be 1.
          if(sum & 0xF00){
            V[0xF] = 1;
          }else{
            V[0xF] = 0;
          }
          // truncate to only the 8 bits.
          V[(opcode & 0x0F00) >> 8] = (unsigned char) sum;
        break;
        }
        case 0x0005:
        {
          // if Vx > Vy, set Vx = Vx - Vy, and VF = not borrow, else Vx = Vy- Vx VF = borrow
          unsigned char x_val = V[(opcode & 0x0F00) >> 8];
          unsigned char y_val = V[(opcode & 0x00F0) >> 4];
          if(x_val > y_val)
          {
            V[0xF] = 0;
          }else{
            V[0xF] = 1; // borrow
          }
          V[(opcode & 0x0F00) >> 8] = y_val - x_val;
          break;
        }
        case 0x0006:
          // if the LSB of Vx is 1, then VF = 1, else 0. then divide Vx by 2
          if(V[(opcode & 0x0F00) >> 8] & 1)
          {
            V[0xF] = 1;
          }else{
            V[0xF] = 0;
          }

          V[(opcode & 0x0F00) >> 8] >>= 2;

        break;
        case 0x0007:
        {

          // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
          unsigned char x_val = V[(opcode & 0x0F00) >> 8];
          unsigned char y_val = V[(opcode & 0x00F0) >> 4];
          if(y_val > x_val)
          {
            V[0xF] = 1;
          }else{
            V[0xF] = 0;
          }

          V[(opcode & 0x0F00) >> 8] = y_val - x_val;
          break;
        }

        case 0x000E:

          // if the MSB bit of Vx = 1, then Vf is set to 1, otherwise 0. then Vx = Vx *2
          if(V[(opcode & 0x0F00) >> 8] & 128){
              V[0xF] = 1;
          }else{
              V[0xF] = 0;
          }

          V[(opcode & 0x0F00) >> 8] <<= 2;

        break;

      }

    break;
    }

    case 0x9000:
    {
    break;
    } 
  }

  PC+=2;
  return 0;

  }else{
    // finished running program in memory
    return -1;
  }
}

void exit(){
  exit(0);
}


void dump_memory(){
  // dumps memory with 20 bytes per line.
  for(unsigned short i=512,count=1;i<4096;i++,count++)
  {
    printf("%X ", memory[i]);
    if(count%20 == 0)
    {
      printf("%c", '\n');
    }

  }
}

void instruction_error(unsigned short instruction){
  fprintf(stderr, "Unknown Instruction %X\n", instruction);
}