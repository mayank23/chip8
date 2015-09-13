#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chip8.h"
#include "../gui/game_window.h"
#include "../gui/key_press.h"
#define FONT_SET_COUNT 80

SDL_Window *game_window = NULL;
SDL_Renderer* game_window_renderer = NULL;
unsigned char screen_pixels[WINDOW_HEIGHT][WINDOW_WIDTH];
Point points[WINDOW_HEIGHT][WINDOW_WIDTH];

int initialize_core(){

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
  
  unsigned char letters[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // load font into memory.
  unsigned short m_idx = 0;
  for(int i=0;i<FONT_SET_COUNT;i++)
  {
    memory[m_idx] = letters[i];
    m_idx++;
  }
  
  // intialize random number generation with seed
  srand(time(NULL));

  draw_color background_color;
  background_color.r = 0;
  background_color.g = 0;
  background_color.b = 0;
  background_color.a = 255; // opaque
  setup_game_window(&background_color);
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
        {
        printf("clearing the display\n");
          // clear the display
        clear_screen();
        break;
        }
        case 0x00EE:{
        printf("return from subroutine, set PC = %X\n", stack[SP]);
          // return from subroutine
          // set PC to addr at top of stack.
          PC = stack[SP];
          stack[SP] = 0; // clear out stack entry.
          SP--; // decrement stack pointer
          PC+=2;// next instruction.
        return 0;
        }
      }

      break;
    }

    case 0x1000:
    {
      // jump to location nnn
      unsigned short jump_address = (opcode & 0x0FFF);
      PC = jump_address;
      printf("jumped to address %X in memory\n", jump_address);
      return 0;
    }

    case 0x2000:
    {
      // call subroutine at address nnn
      unsigned short subroutine_address = opcode & 0x0FFF;
      SP++; // new spot on the stack.
      stack[SP] = PC;
      PC = subroutine_address;
      printf("called subroutine at address %X in memory\n", subroutine_address);
    return 0;
    }

    case 0x3000:
    {
      // 3xkk
      // skip next instruction if Vx = kk
      if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
        printf("Skipping next instruction\n");
        PC += 4; // each instruction is 2 bytes.
        return 0;
      }
    break;
    }

    case 0x4000:{
      // 4xkk
      // if Vx != kk skip the next instruction
     printf("Skipping next instruction\n");
      if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
      {
        PC += 4;
        return 0;
      }
    break;
  }

    case 0x5000:{
      // 5xy0
      // if vx and xy are equal, skip the next instruction
     printf("Skipping next instruction\n");
      if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
      {
        PC += 4;
        return 0;
      }
    break;
  }

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
      // if Vx != Vy, skip next instruction.
      //9xy0
      if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
      {
        PC+=4;
      }
      return 0;
    } 

    case 0xA000:
    {
      // Annn, set I = nnn
      I = (opcode & 0x0FFF);
      break;
    }

    case 0xB000:
    {
      // jump to address nnn in ram + V[0]
      PC = (opcode & 0x0FFF) + V[0];
      break;
    }

    case 0xC000:
    {

      V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);

      break;
    }

    case 0XD000:
    {
      
      unsigned char n_bytes = (opcode & 0x000F);
      unsigned char x_origin = V[(opcode & 0x0F00) >> 8];
      unsigned char y_origin = V[(opcode & 0x00F0) >> 4];

      for(unsigned short mem_index = I, y_offset = 0;mem_index < I + n_bytes; mem_index++, y_offset++)
      {
        unsigned char current_byte = memory[mem_index];

        for(int shift=7, x_offset = 0; shift>=0; shift--, x_offset++)
        {
          // for right now do not wrap over screen.
          
          unsigned char current_bit = (current_byte >> shift) & 1;
          // modulo width and height for wrapping.
          if((screen_pixels[(y_origin + y_offset)%WINDOW_WIDTH][(x_origin + x_offset)%WINDOW_HEIGHT] ^  current_bit) == 0 && current_bit!=0)
          {
            V[0xF] = 1; // collision
          }
          // pixel state for draw.
          screen_pixels[y_origin + y_offset][x_origin + x_offset] ^= current_bit;
          
        }
      }
      draw_to_screen(x_origin, y_origin, n_bytes);
      break;
    }

    case 0xE000:{
      switch(opcode & 0x000F){
      
        case 0x000E:{
          // key is pressed
          if(key_press_state[V[(opcode & 0x0F00) >> 8]] == 1)
          {
            printf("$$KEY (%X) was pressed, skipping instruction\n",V[(opcode & 0x0F00) >> 8]);
            PC +=4;
            return 0;
          }

          break;
        }
        // key is not pressed.
        case 0x0001:{

          if(key_press_state[V[(opcode & 0x0F00) >> 8]] != 1)
          {
              printf("$$KEY(%X) was not pressed, skipping instruction\n",V[(opcode & 0x0F00) >> 8]);
            PC +=4;
            return 0;
          }

        break;
        }
      }

      break;
    }

    case 0xF000:{
      switch(opcode & 0x00FF)
      {
        case 0x0007:{
          V[(opcode & 0x0F00) >> 8] = delay;
          break;
        }
        case 0x000A:{
          // wait for a key press.
          printf("Waiting for key press...\n");
          while(!get_key_press()){
            // keep waiting.
          }
          int key_index = -1;
          for(int i=0;i<16;i++)
          {
            if(key_press_state[i])
            {
              key_index = i;
              break;
            }
          }
          V[(opcode & 0x0F00) >> 8] = key_index;

          break;
        }
        case 0x0015:{
          delay = V[(opcode & 0x0F00) >> 8];
          break;
        }
        case 0x0018:{
          sound = V[(opcode & 0x0F00) >> 8];
          break;
        }
        case 0x001E:{
          I += V[(opcode & 0x0F00) >> 8];
          break;
        }
        case 0x0029:{
          unsigned char letter = V[(opcode & 0x0F00) >> 8];
          printf("FONT LETTER WANTED: %X\n", letter);
          I = letter * 5;
          if(I > 4096)
          {
            I = 0;
          }
          break;
        }
        case 0x0033:{
          //store BCD representation of Vx in memory locations, I, I+1, I+2.
          unsigned char value = V[(opcode & 0x0F00) >> 8];
          memory[I+2] = value % 10;
          value /= 10;
          memory[I+1] = value % 10;
          value /= 10;
          memory[I] = value % 10;
          break;
        }
        case 0x0055:{
          unsigned short temp = I;
          unsigned char register_range  = (opcode & 0x0F00) >> 8;
          for(unsigned char curr = 0;curr<=register_range;curr++)
          {
            memory[I] = V[curr];
            I++;
          }

          I = temp;

          break;
        }
        case 0x0065:{
          unsigned short temp = I;
          unsigned char register_range = (opcode & 0x0F00) >> 8;
          for(unsigned char curr = 0;curr <= register_range;curr++)
          {
            V[curr] = I;
            I++;
          }
          // restore I
          I = temp;

          break;
        }

      }
      break;
    }
  }
  if(delay > 0){
    printf("delay: %d\n", delay);
  delay--;
  }
  if(sound > 0)
  {
  sound--;
  }
  PC+=2;
  return 0;

  }else{
    // finished running program in memory
    return -1;
  }
}

// draws the area of the screen that was updated.
void clear_screen()
{
  draw_color color;
  color.r = 0;
  color.g = 0;
  color.b = 0;
  color.a = 255;
  clear_window(&color);
}

void draw_to_screen(unsigned char x_origin, unsigned char y_origin, unsigned char n_bytes){
  for(int i=y_origin;i<y_origin + n_bytes;i++)
  {
    for(int j=x_origin;j<x_origin + 8;j++)
    {
      draw_color color;
      if(screen_pixels[i][j])
      {
        // draw white.
        color.r = 255;
        color.g = 255;
        color.b = 255;
        color.a = 255;
        draw_pixel(&points[i][j], &color);
      }else{
        // black
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        draw_pixel(&points[i][j], &color);
      }
    }
  }
}


void exit_core(){
  destroy_game_window();
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