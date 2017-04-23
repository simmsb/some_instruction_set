#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DEFAULT_SIZE 1<<16 // 2^16 bytes of ram

struct Cpu *init_cpu(int mem_size) {
  struct Cpu *cpu = malloc(sizeof(struct Cpu));
  cpu->memory = calloc(mem_size, sizeof(uint16_t));
  cpu->flags.halt = true;
  cpu->regs[esp] = mem_size;
  return cpu;
}

void load_program(struct Cpu *cpu, uint16_t *program, int length) {
  for (int i=0; i < length; i++)
    cpu->memory[i] = program[i];
}

void run_cmd(struct Cpu *cpu) {
  struct PackedInstr instr = decode(cpu);
  instr.i(cpu, instr.arg1, instr.arg2);
}

void run(struct Cpu *cpu) {
  puts("Starting cpu");
  while (cpu->flags.halt) {
    run_cmd(cpu);
  }
  puts("Closing cpu");
}

uint16_t hex_value(char val) {
  switch (val) {
    case '0' ... '9':
      return val - '0';
    case 'A' ... 'F':
      return 10 + (val - 'A');
    case 'a' ... 'f':
      return 10 + (val - 'a');
    default:
      return 0;
  }
}

uint16_t *parse_hex(char *hex) {
  puts(hex);
  int length = strlen(hex);
  uint16_t *prog = calloc(length/4, sizeof(uint16_t));
  for (int i=0; i<length/4; i++) {
    for (int k=0; k<4; k++) {
      prog[i] += pow(16, 3-k) * hex_value(hex[(i*4)+k]);
    }
  }
  return prog;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("One argument expected. USAGE: %s <program>\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct Cpu *cpu = init_cpu(DEFAULT_SIZE);
  uint16_t *program = parse_hex(argv[1]);

  load_program(cpu, program, strlen(argv[1])/2);
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

// print `Wew\n` -> 0x4007005740070065400700774007000A0003
