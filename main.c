#include <stdio.h>
#include <stdlib.h>
#include "core/chip8.h"


int main(int argc, char** argv)
{
  initialize_core();
  load_rom("./games/PONG");
  dump_memory();
  /*while(emulate_cycle() != -1)
  {

  }*/
  exit_core();

  return 0;
}