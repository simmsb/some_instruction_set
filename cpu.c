#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_SIZE 1<<16 // 2^16 bytes of ram

struct Cpu *init_cpu(int mem_size) {
  struct Cpu *cpu = malloc(sizeof(struct Cpu));
  cpu->memory = calloc(mem_size, sizeof(char));
  cpu->instructions = cpu->memory;
  cpu->flags.halt = true;
  cpu->regs.esp = mem_size;
  return cpu;
}

void load_program(struct Cpu *cpu, char *program) {
  char *base = cpu->memory; // copy location of cpu ram
  while (*program++) { // load program into bottom of memory
    *base = *program;
    ++base; // ooh this is gonna give some sigsegvs isn't it
  }
}

void run_cmd(struct Cpu *cpu) {
  struct PackedInstr instr = decode(&(cpu->memory));
  instr.i(cpu, instr.args);
}

void run(struct Cpu *cpu) {
  while (cpu->flags.halt) {
    run_cmd(cpu);
  }
}

int main() {
  struct Cpu *cpu = init_cpu(DEFAULT_SIZE);
  run(cpu);
  return 0;
}
