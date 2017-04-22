#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_SIZE 1<<16 // 2^16 bytes of ram

struct Cpu *init_cpu(int mem_size) {
  struct Cpu *cpu = malloc(sizeof(struct Cpu));
  cpu->memory = calloc(mem_size, sizeof(uint16_t));
  cpu->flags.halt = true;
  cpu->regs[esp] = mem_size;
  return cpu;
}

void load_program(struct Cpu *cpu, uint16_t *program) {
  uint16_t *base = cpu->memory; // copy location of cpu ram
  while (*program++) { // load program into bottom of memory
    *base = *program;
    ++base; // ooh this is gonna give some sigsegvs isn't it
  }
}

void run_cmd(struct Cpu *cpu) {
  struct PackedInstr instr = decode(cpu);
  instr.i(cpu, instr.arg1, instr.arg2);
}

void run(struct Cpu *cpu) {
  while (cpu->flags.halt) {
    run_cmd(cpu);
  }
}

int main() {
  struct Cpu *cpu = init_cpu(DEFAULT_SIZE);
  uint16_t program[9] = {0x4007, 0x57, 0x4007, 0x65, 0x4007, 0x77, 0x4007, 0xa, 0x3};
  for (int i=0; i<9; i++) {
    printf("%" PRIu16 "\n", program[i]);
  }
  printf("\n");
  load_program(cpu, program);
  run(cpu);
  return 0;
}

void cpu_setreg(struct Cpu *cpu, uint16_t reg, uint16_t value) {
  cpu->regs[reg] = value;
}

uint16_t cpu_getloc(struct Cpu *cpu, uint16_t location) {
  uint16_t num;

  if (location & 0x4000) { // if register
    int regnum = ((location & 0x3C00) >> 10) & 0xF;
    num = cpu->regs[regnum];
  } else { // abs
    num = location & 0x3FFF;
  }
  return (location & 0x8000)?*(cpu->memory + num):num;
}

inline uint16_t cpu_popstack(struct Cpu *cpu) {
  return cpu->memory[++cpu->regs[esp]]; // move up, get value
}

inline void cpu_pushstack(struct Cpu *cpu, uint16_t val) {
  cpu->memory[cpu->regs[esp]--] = val; // set value, move down
}
