#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

int main(int argc, char** argv)
{
  initialize();
  load_rom("../games/PONG");
  dump_memory();

  return 0;
}