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
  uint16_t *memory;
  uint16_t *instructions; // references to base of memory at start, but moves with instructions
  struct Registers regs;
  struct Flags flags;
};

typedef void (*instruction)(struct Cpu *, uint16_t);

instruction  noArgs(uint16_t);
instruction  oneArg(uint16_t);
instruction twoArgs(uint16_t);

struct PackedInstr {
  instruction i;
  int n;
  uint16_t arg1;
  uint16_t arg2;
};

struct PackedInstr decode(uint16_t **);

void nop(struct Cpu *);
void ret(struct Cpu *);
void call(struct Cpu *);

void jmp(struct Cpu *, uint16_t); // short jump (char)
void psh(struct Cpu *, uint16_t);
void pop(struct Cpu *, uint16_t);

void tst(struct Cpu *, uint16_t, uint16_t);
void mov(struct Cpu *, uint16_t, uint16_t);
