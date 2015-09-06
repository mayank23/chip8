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

/* stack should store the Program Counter (index of the first byte of the instruction in memory) to come back later.*/
unsigned short stack[16];

/* last byte of rom address. */
unsigned short rom_end_addr;

int initialize_core();
int load_rom(char* rom_path);
int emulate_cycle();
void exit_core();
/* Debug Methods */
void dump_memory();
void dump_registers();
void instruction_error(unsigned short instruction);