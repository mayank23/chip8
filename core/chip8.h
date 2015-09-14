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

/* opcodes*/
int opc_0(unsigned short opcode);
int opc_1(unsigned short opcode);
int opc_2(unsigned short opcode);
int opc_3(unsigned short opcode);
int opc_4(unsigned short opcode);
int opc_5(unsigned short opcode);
int opc_6(unsigned short opcode);
int opc_7(unsigned short opcode);
int opc_8(unsigned short opcode);
int opc_9(unsigned short opcode);
int opc_A(unsigned short opcode);
int opc_B(unsigned short opcode);
int opc_C(unsigned short opcode);
int opc_D(unsigned short opcode);
int opc_E(unsigned short opcode);
int opc_F(unsigned short opcode);

/* drawing methods */
void draw_to_screen(unsigned char, unsigned char, unsigned char);
void clear_screen();

/* Debug Methods */
void dump_memory();
void dump_registers();
void initialize_interactive_mode();
void load_single_instruction_into_memory(unsigned short addr, unsigned short instruction);
void instruction_error(unsigned short instruction);

