/* RAM 4KB */
unsigned char memory[4096];

/* Registers */
unsigned char V[16];

unsigned char sound, delay;

/* each instruction is 2 bytes. PC stores currently executing address */

unsigned short PC, I;

/* Stack Pointer, points to top most level of stack */
unsigned char SP;

/* stack should store address where interpreter should return to */
unsigned short stack[16];

void initialize();
void run(char* rom_path);
