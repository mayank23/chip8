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
  }else
  if(strcmp(game_path, "interactive") == 0){
    // interactive  mode.
    initialize_interactive_mode();
    unsigned short current_addr = 0x200;
    while(current_addr < 0xFFF)
    {
      unsigned short instruction =0;
      scanf("%hx",&instruction);
      if(instruction == 0)
      {
        exit_core();
        return 0;
      }
      printf("instruction loaded into memory\n");
      load_single_instruction_into_memory(current_addr, instruction);
      emulate_cycle();
      current_addr+=2;
    }
  }else{
    load_rom(game_path);
    while(emulate_cycle() != -1)
    { 
    int ret = get_key_press();
    }
    exit_core();

  }
  return 0;
}


