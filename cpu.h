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

struct PackedInstr {
  instruction i;
  uint16_t args;
};

struct PackedInstr decode(char **);
