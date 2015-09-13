#include <stdio.h>
#include <stdlib.h>
#include "core/chip8.h"
#include <SDL.h>
#include "gui/key_press.h"

#define DEBUG 0
unsigned char key_press_state[16] = {0};

int main(int argc, char** argv)
{
  initialize_core();
  char* game_path = argv[1];
  if(game_path == NULL)
  {
    printf("Please enter game path through command line arg.\n");
    return 1;
  }
  load_rom(game_path);
  dump_memory();
  while(emulate_cycle() != -1)
  {
    int ret = get_key_press();
  }
  exit_core();

  return 0;
}


