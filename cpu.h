#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#define NUM_REGS 11

typedef struct Node Node;
struct Cpu;

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
  rip = 9,
  acc = 10,
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
  float fregs[NUM_REGS];
  struct Flags flags;
  Node *interrupts;
  uint64_t ticks;
};

struct Interrupt {
  void (*callback)(struct Cpu *);
  uint64_t when;
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
void check_interrupts(struct Cpu *);

void nop(struct Cpu *, uint16_t, uint16_t);
void ret(struct Cpu *, uint16_t, uint16_t);
void call(struct Cpu *, uint16_t, uint16_t);
void halt(struct Cpu *, uint16_t, uint16_t);

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

// math operations (2 args)
void add(struct Cpu *, uint16_t, uint16_t);
void sub(struct Cpu *, uint16_t, uint16_t);
void mul(struct Cpu *, uint16_t, uint16_t);
void divn(struct Cpu *, uint16_t, uint16_t);
void rem(struct Cpu *, uint16_t, uint16_t);
void flc(struct Cpu *, uint16_t, uint16_t);
void clf(struct Cpu *, uint16_t, uint16_t);
void stf(struct Cpu *, uint16_t, uint16_t);
void ldf(struct Cpu *, uint16_t, uint16_t);
void mvf(struct Cpu *, uint16_t, uint16_t); // move float between regs
void fad(struct Cpu *, uint16_t, uint16_t); // add freg arg1, freg arg2 store in float reg acc
void fsb(struct Cpu *, uint16_t, uint16_t);
void fmu(struct Cpu *, uint16_t, uint16_t);
void fdv(struct Cpu *, uint16_t, uint16_t);

typedef struct Node {
  Node *next;
  Node *prev;
  struct Interrupt *irq;
} Node;

void free_node(Node **, Node *);
void push_node(Node **, struct Interrupt *);
