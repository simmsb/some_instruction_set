#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
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
  bool carry : 1;
  bool parity : 1;
  bool zero : 1;
  bool sign : 1;
  bool overflow : 1;
  bool halt : 1;
};

struct Cpu {
  uint16_t *memory;
  uint16_t regs[NUM_REGS];
  float fregs[NUM_REGS];
  struct Flags flags;
  Node *interrupts;
  uint64_t ticks;
  uint64_t extreg;
};

struct Interrupt {
  void (*callback)(struct Cpu *, uint16_t *);
  uint16_t when;
  uint16_t data[1]; // length of this == number of args
};

typedef void (*instruction)(struct Cpu *, uint16_t, uint16_t);

#define OPERATION_I struct Cpu *cpu, uint16_t arg1, uint16_t arg2

instruction noArgs(uint16_t);
instruction oneArg(uint16_t);
instruction twoArgs(uint16_t);

struct PackedInstr {
  instruction i;
  uint16_t arg1;
  uint16_t arg2;
};

void decode(struct Cpu *, struct PackedInstr *);

uint16_t cpu_getloc(struct Cpu *, uint16_t);
uint16_t cpu_popstack(struct Cpu *);
void cpu_pushstack(struct Cpu *, uint16_t);
void cpu_setreg(struct Cpu *, uint16_t, uint16_t);
void cpu_setloc(struct Cpu *, uint16_t, uint16_t);
void check_interrupts(struct Cpu *);

// 0 arg ops
void nop(struct Cpu *, uint16_t, uint16_t); // 0x0000
void ret(struct Cpu *, uint16_t, uint16_t);
void call(struct Cpu *, uint16_t, uint16_t);
void halt(struct Cpu *, uint16_t, uint16_t);

// 1 arg ops
void jmp(struct Cpu *, uint16_t, uint16_t);
void jeq(struct Cpu *, uint16_t, uint16_t);
void jne(struct Cpu *, uint16_t, uint16_t);
void jle(struct Cpu *, uint16_t, uint16_t);
void jme(struct Cpu *, uint16_t, uint16_t);
void psh(struct Cpu *, uint16_t, uint16_t);
void pop(struct Cpu *, uint16_t, uint16_t);
void ptc(struct Cpu *, uint16_t, uint16_t);

// 2 arg ops
void tst(struct Cpu *, uint16_t, uint16_t);
void mov(struct Cpu *, uint16_t, uint16_t);
void irq(struct Cpu *, uint16_t, uint16_t);

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
void fad(struct Cpu *, uint16_t,
         uint16_t); // add freg arg1, freg arg2 store in float reg acc
void fsb(struct Cpu *, uint16_t, uint16_t);
void fmu(struct Cpu *, uint16_t, uint16_t);
void fdv(struct Cpu *, uint16_t, uint16_t);
void addl(struct Cpu *, uint16_t, uint16_t);
void subl(struct Cpu *, uint16_t, uint16_t);
void mull(struct Cpu *, uint16_t, uint16_t);
void divl(struct Cpu *, uint16_t, uint16_t);
void reml(struct Cpu *, uint16_t, uint16_t);
void addq(struct Cpu *, uint16_t, uint16_t);
void subq(struct Cpu *, uint16_t, uint16_t);
void mulq(struct Cpu *, uint16_t, uint16_t);
void divq(struct Cpu *, uint16_t, uint16_t);
void remq(struct Cpu *, uint16_t, uint16_t);
void not(struct Cpu *, uint16_t, uint16_t);
void and(struct Cpu *, uint16_t, uint16_t);
void or(struct Cpu *, uint16_t, uint16_t);
void xor(struct Cpu *, uint16_t, uint16_t);






// IRQ stuff
typedef struct Node {
  Node *next;
  Node *prev;
  struct Interrupt *irq;
} Node;

void free_node(Node **, Node *);
void push_node(Node **, struct Interrupt *);

void timed_jump(struct Cpu *, uint16_t *);

void schedule(struct Cpu *, struct Interrupt *);
void deschedule(struct Cpu *, Node *);
