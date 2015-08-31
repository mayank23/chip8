#include <string.h>
#include <stdio.h>
#include "chip8.h"

int initialize(){

  /* clear all registers and memory */

  memset(memory, 0, 4096);
  memset(V,0,16);
  I = 0;
  PC = 0x200; // program counter starts from byte 512.
  SP = 0;
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


    break;

    case 0x1000:
    break;

    case 0x2000:
    break;

    case 0x3000:
    break;

    case 0x4000:
    break;

    case 0x5000:
    break;

    case 0x6000:
      /*Set Vx = kk.*/
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
      //printf("Load register: %X with %X\n", (opcode & 0x0F00) >> 8, opcode & 0x00FF); 
      break;

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