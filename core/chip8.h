/* RAM 4KB */
unsigned char memory[4096];

/* General Purpose Registers */
unsigned char V[16];

// timing registers
unsigned char sound, delay;

/* each instruction is 2 bytes with Big Endian representation.
 PC stores currently executing address 
I stores memory addresses.
*/
unsigned short PC, I;

/* Stack Pointer, points to top most level of stack */
unsigned char SP;

/* stack should store address where interpreter should return to */
unsigned short stack[16];

int initialize();
int load_rom(char* rom_path);
int emulate_cycle();
void exit();
/* Debug Methods */
void dump_memory();
void dump_registers();