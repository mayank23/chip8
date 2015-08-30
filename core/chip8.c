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

  while(fread((void*)buffer, 1, 256, rom_file) != 0)
  {
    for(int i=0;i<256 && m_index < 4096;i++,m_index++)
    {
      memory[m_index] = buffer[i];
    }
  }
  printf("%s %s\n", rom_path, "loaded");
  return 0;
}

int emulate_cycle(){
  // each instruction is 16 bits long, 2 bytes.
  unsigned short opcode = (memory[PC] << 8) + memory[PC+1];
  return 0;
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