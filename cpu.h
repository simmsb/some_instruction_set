struct Registers {
  int eax, rax, epb, rsp, aaa, bbb;
};

struct Cpu {
  char *mem;
  struct Registers regs;
};

typedef void (*instruction)(struct Cpu *cpu, unsigned int args);

struct PackedInstr {
  instruction i;
  unsigned int args;
};
