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
draw_color black_color;
draw_color white_color;

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
  
  black_color.r = 0;
  black_color.g = 0;
  black_color.b = 0;
  black_color.a = 255; // opaque
 
  white_color.r = 255;
  white_color.g = 255;
  white_color.b = 255;
  white_color.a = 255; // opaque

  setup_game_window(&black_color);
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
      opc_0(opcode);
      break;
    }

    case 0x1000:
    {
      opc_1(opcode);
      break;
    }

    case 0x2000:
    {
     opc_2(opcode);
     break;
    }

    case 0x3000:
    {
      opc_3(opcode);
      break;
    }

    case 0x4000:{
     opc_4(opcode);
     break;
    }

    case 0x5000:{
      opc_5(opcode);
      break;
    }

    case 0x6000:{
      opc_6(opcode);
      break;
    }

    case 0x7000:{
      opc_7(opcode);
      break;
    }

    case 0x8000:{
      opc_8(opcode);
      break;
    }

    case 0x9000:
    {
     opc_9(opcode);
     break;
    } 

    case 0xA000:
    {
      opc_A(opcode);
      break;
    }

    case 0xB000:
    {
      opc_B(opcode);
      break;
    }

    case 0xC000:
    {
      opc_C(opcode);
      break;
    }

    case 0XD000:
    {
      opc_D(opcode);
      break;
    }

    case 0xE000:{
      opc_E(opcode);
      break;
    }

    case 0xF000:{
      opc_F(opcode);
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
  
  return 0;

  }else{
    // finished running program in memory
    return -1;
  }
}

int opc_0(unsigned short opcode){
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
      break;
    }
  }
  // next instruction
  PC+=2;
  return 0;
}
int opc_1(unsigned short opcode){
  // jump to location nnn
  unsigned short jump_address = (opcode & 0x0FFF);
  PC = jump_address;
  printf("jumped to address %X in memory\n", jump_address);
  return 0;
}
int opc_2(unsigned short opcode){
  // call subroutine at address nnn
  unsigned short subroutine_address = opcode & 0x0FFF;
  SP++; // new spot on the stack.
  stack[SP] = PC;
  PC = subroutine_address;
  printf("called subroutine at address %X in memory\n", subroutine_address);
  return 0;
}
int opc_3(unsigned short opcode){
  if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
    printf("Skipping next instruction\n");
    PC += 4; // each instruction is 2 bytes.
  }else{
    PC+=2;
  }
  return 0;
}
int opc_4(unsigned short opcode){
  // 4xkk
  // if Vx != kk skip the next instruction
  printf("Skipping next instruction\n");
  if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
  {
    PC += 4;
  }else{
    PC+=2;
  }
  return 0;
}
int opc_5(unsigned short opcode){
  // 5xy0
  // if vx and xy are equal, skip the next instruction
  printf("Skipping next instruction\n");
  if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
  {
    PC += 4;
  }else{
    PC+=2;
  }
  return 0;
}
int opc_6(unsigned short opcode){
  /*Set Vx = kk.*/
  V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
  printf("Load register: %hu with %hu\n", (unsigned short)((opcode & 0x0F00) >> 8), (unsigned short)(opcode & 0x00FF)); 
  PC+=2;
  return 0;
}
int opc_7(unsigned short opcode){
  //7xkk 
  // adds kk to register Vx and stores it there.
  printf("Adding %hu to register: %hu\n",(unsigned short) ((opcode & 0x0F00) >> 8),(unsigned short) (opcode & 0x00FF));
  V[(opcode & 0x0F00) >> 8]+= (opcode & 0x00FF);
  PC+=2;
  return 0;

}
int opc_8(unsigned short opcode){
  // 8xyn
  printf("Math operations with Registers: %u, %u\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
  switch(opcode & 0x000F){

    case 0x0001:{
      // set Vx = Vx | Vy.
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];

    break;
    }
    case 0x0002:{
      // set Vx = Vx & Vy
     V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
     break;
    }

    case 0x0003:{
      // set Vx = Vx ^ Vy
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
      break;
    }

    case 0x0004:
    {
      // set Vx = Vx + Vy, set VF = carry.
      unsigned short sum = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
      // if there was a carry, then the 9th bit would be 1.
      if(sum > 255){
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
      V[(opcode & 0x0F00) >> 8] = x_val - y_val;
      break;
    }

    case 0x0006:{
      // if the LSB of Vx is 1, then VF = 1, else 0. then divide Vx by 2
      if(V[(opcode & 0x0F00) >> 8] & 1)
      {
        V[0xF] = 1;
      }else{
        V[0xF] = 0;
      }

      V[(opcode & 0x0F00) >> 8] >>= 1;

      break;
    }

    case 0x0007:
    {


      unsigned char x_val = V[(opcode & 0x0F00) >> 8];
      unsigned char y_val = V[(opcode & 0x00F0) >> 4];
      if(y_val > x_val)
      {
        V[0xF] = 1;
      }else{
        // borrow
        V[0xF] = 0;
      }

      V[(opcode & 0x0F00) >> 8] = y_val - x_val;
      break;
    }

    case 0x000E:{

      // if the MSB bit of Vx = 1, then Vf is set to 1, otherwise 0. then Vx = Vx *2
      if(V[(opcode & 0x0F00) >> 8] & 128){
          V[0xF] = 1;
      }else{
          V[0xF] = 0;
      }

      V[(opcode & 0x0F00) >> 8] <<= 1;

      break;
    }

  }

  PC+=2;
  return 0;
}
int opc_9(unsigned short opcode){
  // if Vx != Vy, skip next instruction.
  //9xy0
  if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
  {
    PC+=4;
  }else{
    PC+=2;
  }

  return 0;
}
int opc_A(unsigned short opcode){
  // Annn, set I = nnn
  I = (opcode & 0x0FFF);
  PC+=2;
  return 0;
}
int opc_B(unsigned short opcode){
  // jump to address nnn in ram + V[0]
  PC = (opcode & 0x0FFF) + V[0];
  return 0;
}

int opc_C(unsigned short opcode){
  V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
  PC+=2;
  return 0;
}

int opc_D(unsigned short opcode){

  unsigned char n_bytes = (opcode & 0x000F);
  unsigned char x_origin = V[(opcode & 0x0F00) >> 8];
  unsigned char y_origin = V[(opcode & 0x00F0) >> 4];
  unsigned char current_bit;
  unsigned int y_pixel_index,x_pixel_index;
  V[0XF]= 0;
  for(unsigned short mem_index = I, y_offset = 0;mem_index < I + n_bytes; mem_index++, y_offset++)
  {
    unsigned char current_byte = memory[mem_index];

    for(int shift=7, x_offset = 0; shift>=0; shift--, x_offset++)
    {
      
       current_bit = (current_byte >> shift) & 1;
       y_pixel_index = (y_origin + y_offset) % WINDOW_HEIGHT;
       x_pixel_index = (x_origin + x_offset) % WINDOW_WIDTH;
      // modulo width and height for wrapping.
      if((screen_pixels[y_pixel_index][x_pixel_index] ^  current_bit) == 0 && current_bit == 1)
      {
        V[0xF] = 1; // collision
      }
      screen_pixels[y_pixel_index][x_pixel_index] ^=  current_bit;
      draw_color* color;
      if( screen_pixels[y_pixel_index][x_pixel_index] == 1)
      {
        color = &white_color;
      }else{
        color = &black_color;
      }

      draw_pixel(x_pixel_index*SCALE,y_pixel_index*SCALE, color);
    }
  }
  PC+=2;
  return 0;
}
int opc_E(unsigned short opcode){
  switch(opcode & 0x000F){
    // key is pressed
    case 0x000E:{
      if(key_press_state[V[(opcode & 0x0F00) >> 8]] == 1)
      {
        printf("$$KEY (%X) was pressed, skipping instruction\n",V[(opcode & 0x0F00) >> 8]);
        PC +=4;
      }else{
        PC+=2;
      }
      break;
    }

    // key is not pressed.
    case 0x0001:{
      if(key_press_state[V[(opcode & 0x0F00) >> 8]] != 1)
      {
        printf("$$KEY(%X) was not pressed, skipping instruction\n",V[(opcode & 0x0F00) >> 8]);
        PC +=4;
      }else{
        PC+=2;
      }
      break;
    }
  }

  return 0;
}

int opc_F(unsigned short opcode){
  switch(opcode & 0x00FF)
  {
    case 0x0007:{
      V[(opcode & 0x0F00) >> 8] = delay;
      break;
    }
    case 0x000A:{
      // wait for a key press.
      printf("Waiting for key press...\n");
      int key_index = -1;
      for(int i=0;i<16;i++)
      {
        if(key_press_state[i])
        {
          key_index = i;
          break;
        }
      }
      if(key_index < 0)
      {
        // try again in next cycle.
        PC-=2; // to prevent going to next instruction.
      }else{
      V[(opcode & 0x0F00) >> 8] = key_index;
      }
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

      unsigned char register_range  = (opcode & 0x0F00) >> 8;
      for(unsigned char curr = 0;curr<=register_range;curr++)
      {
        memory[I] = V[curr];
        I++;
      }


      break;
    }
    case 0x0065:{

      unsigned char register_range = (opcode & 0x0F00) >> 8;
      for(unsigned char curr = 0;curr <= register_range;curr++)
      {
        V[curr] = I;
        I++;
      }
      break;
    }

  }

  PC+=2;
  return 0;
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
void load_single_instruction_into_memory(unsigned short addr, unsigned short instruction){
  // instructions stored in big endian represenation.
  if(addr < 4095){
    unsigned char byte_1 = (instruction & 0xFF00) >> 8;
    unsigned char byte_2 = (instruction & 0x00FF);
    memory[addr] = byte_1;
    memory[addr+1] = byte_2;
  }else{
    // cant store 2 bytes, will cause buffer overflow.
  }
}

void initialize_interactive_mode(){
  // set rom_end_addr all the way to the end.
  rom_end_addr = 0xFFF; // end is byte 4095
}

void instruction_error(unsigned short instruction){
  fprintf(stderr, "Unknown Instruction %X\n", instruction);
}