#include <stdint.h>
#include <stdbool.h>

struct Registers {
  uint16_t aaa, //─┐
           bbb, // │
           ccc, // │
           ddd, // ├─ general purpose registers
           eee, // │
           fff, // │
           ggg, //─┘
           esp, // stack pointer
           epb, // base pointer
           rip; // instruction pointer
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
  struct Registers regs;
  struct Flags flags;
};

typedef void (*instruction)(struct Cpu *, uint16_t, uint16_t);

#define OPERATION_I struct Cpu *cpu, uint16_t arg1, uint16_t arg2

instruction  noArgs(uint16_t);
instruction  oneArg(uint16_t);
instruction twoArgs(uint16_t);

struct PackedInstr {
  instruction i;
  int n;
  uint16_t arg1;
  uint16_t arg2;
};

struct PackedInstr decode(struct Cpu *);

void nop(struct Cpu *, uint16_t, uint16_t);
void ret(struct Cpu *, uint16_t, uint16_t);
void call(struct Cpu *, uint16_t, uint16_t);

void jmp(struct Cpu *, uint16_t, uint16_t);
void jeq(struct Cpu *, uint16_t, uint16_t);
void jle(struct Cpu *, uint16_t, uint16_t);
void jme(struct Cpu *, uint16_t, uint16_t);
void psh(struct Cpu *, uint16_t, uint16_t);
void pop(struct Cpu *, uint16_t, uint16_t);

void tst(struct Cpu *, uint16_t, uint16_t);
void mov(struct Cpu *, uint16_t, uint16_t); // move memory at location arg1 to location at arg2
