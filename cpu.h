#include <stdint.h>
#include <stdbool.h>

struct Registers {
  int aaa, //─┐
      bbb, // │
      ccc, // │
      ddd, // ├─ general purpose registers
      eee, // │
      fff, // │
      ggg, //─┘
      esp, // stack pointer
      epb; // base pointer
};

struct Flags {
  bool carry:1;
  bool parity:1;
  bool zero:1;
  bool sign:1;
  bool overflow:1;
  bool halt:1;
};

struct Cpu {
  char *memory;
  char *instructions; // references to base of memory at start, but moves with instructions
  struct Registers regs;
  struct Flags flags;
};

typedef void (*instruction)(struct Cpu *, uint16_t);

instruction  noArgs(char);
instruction  oneArg(char);
instruction twoArgs(char);

struct PackedInstr {
  instruction i;
  uint16_t args;
};

struct PackedInstr decode(char **);

#define OPERATION struct Cpu *, uint16_t
#define OPERATION_I struct Cpu *cpu, uint16_t args

void nop(OPERATION);
void ret(OPERATION);
void call(OPERATION);
void ljmp(OPERATION); // long jump (int)

void sjmp(OPERATION); // short jump (char)
void psh(OPERATION);
void pop(OPERATION);

void tst(OPERATION);
void mov(OPERATION);
