#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define NUM_REGS 10

enum Registers { // used for assember, etc
  aaa = 0,
  bbb = 1,
  ccc = 2,
  ddd = 3,
  eee = 4,
  fff = 5,
  ggg = 6,
  esp = 7,
  epb = 8,
  rip = 9
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
  uint16_t regs[NUM_REGS];
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

uint16_t cpu_getloc(struct Cpu *, uint16_t);
uint16_t cpu_popstack(struct Cpu *);
void cpu_pushstack(struct Cpu *, uint16_t);
void cpu_setreg(struct Cpu *, uint16_t, uint16_t);

void nop(struct Cpu *, uint16_t, uint16_t);
void ret(struct Cpu *, uint16_t, uint16_t);
void call(struct Cpu *, uint16_t, uint16_t);

void jmp(struct Cpu *, uint16_t, uint16_t);
void jeq(struct Cpu *, uint16_t, uint16_t);
void jne(struct Cpu *, uint16_t, uint16_t);
void jle(struct Cpu *, uint16_t, uint16_t);
void jme(struct Cpu *, uint16_t, uint16_t);
void psh(struct Cpu *, uint16_t, uint16_t);
void pop(struct Cpu *, uint16_t, uint16_t);
void ptc(struct Cpu *, uint16_t, uint16_t);

void tst(struct Cpu *, uint16_t, uint16_t);
void str(struct Cpu *, uint16_t, uint16_t); // value of arg1 -> memory location at arg2
void lod(struct Cpu *, uint16_t, uint16_t); // value of arg1 -> register of r2
