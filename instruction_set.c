/*

each instruction is of 1 to 3 ints (uint16_t) long.

first two bits decide on length of instruction (00, 01, 10)

1 int: [0 0] [1 int - 2 bits] (single opcodes only)
              opcode

2 int: [0 1] [1 int - 2 bits] [1 int]
               opcode         operand 1

3 int: [1 0] [1 int - 2 bits] [1 int] [1 int]
               opcode      operand 1 operand 2

*/

#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

uint16_t cpu_popstack(struct Cpu *cpu) {
  return cpu->memory[++cpu->regs.esp]; // move up, get value
}

void cpu_pushstack(struct Cpu *cpu, uint16_t val) {
  cpu->memory[cpu->regs.esp--] = val; // set value, move down
}

void cpu_splitargs(uint32_t args, uint16_t *arg1, uint16_t *arg2) {
  *arg1 = (uint16_t) ((args >> 16) & 0xFFFF);
  *arg2 = (uint16_t) (args & 0xFFFF);
}

struct PackedInstr decode(struct Cpu *cpu) {
  struct PackedInstr i;
  uint16_t *stream = cpu->memory + cpu->regs.rip++;

  uint16_t opcode = *stream & 0x3FFF; // remove upper two bits
  int args = (*stream & 0xC000) >> 14;
  switch (args) {
    case 0:
      i.i = noArgs(opcode);
    case 1:
      i.i = oneArg(opcode);
      i.arg1 = *(cpu->memory + cpu->regs.rip++);
      break;
    case 2:
      i.i = twoArgs(opcode);
      i.arg1 = *(cpu->memory + cpu->regs.rip++);
      i.arg2 = *(cpu->memory + cpu->regs.rip++);
    default:
      goto ERROR;
  }

  return i;

  ERROR:
    exit(1);
}

instruction noArgs(uint16_t opcode) {
  switch (opcode) {
    case 0:
      return nop;
    case 1:
      return ret;
    case 2:
      return call;
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 0 args instruction found, ID: %d\n", (int) opcode);
    exit(1);
}

instruction oneArg(uint16_t opcode) {
  switch (opcode) {
    case 0:
      return jmp;
    case 1:
      return psh;
    case 2:
      return pop;
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 1 arg instruction found, ID: %d\n", (int) opcode);
    exit(1);
}

instruction twoArgs(uint16_t opcode) {
  switch (opcode) {
    case 0:
      return tst;
    case 1:
      return mov;
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 2 args instruction found, ID: %d\n", (int) opcode);
    exit(1);
}

void halt(OPERATION_I){
  cpu->flags.halt = false;
}

void nop(OPERATION_I) {
  // yo it's a no op
}

void ret(OPERATION_I) {
  cpu->regs.rip = cpu_popstack(cpu);
}

void call(OPERATION_I) {
  uint16_t loc = cpu_popstack(cpu); // get location to jump to
  uint16_t current = cpu->regs.rip;
  cpu_pushstack(cpu, ++current);

  cpu->regs.rip = loc;
}

void jmp(OPERATION_I) {
  cpu->regs.rip = arg1;
}

uint16_t cpu_getloc(struct Cpu *cpu, uint16_t location) {
  uint16_t num;

  if (location & 0x4000) { // if register
    int regnum = ((location & 0x3C00) >> 10) & 0xF;
    switch (regnum) {
      case 0:
        num = cpu->regs.aaa;
        break
      case 1:
        num = cpu->regs.bbb;
        break;
      case 2:
        num = cpu->regs.ccc;
        break;
      case 3:
        num = cpu->regs.ddd;
        break;
      case 4:
        num = cpu->regs.eee;
        break;
      case 5:
        num = cpu->regs.fff;
        break;
      case 6:
        num = cpu->regs.ggg;
        break;
      case 7:
        num = cpu->regs.esp;
        break;
      case 8:
        num = cpu->regs.epb;
        break;
      case 9:
        num = cpu->regs.rip;
        break;
      default:
        num = 0;
    }
  } else { // abs
    num = location & 0x3FFF;
  }

  if (location & 0x8000) { // if deref
    num = *(cpu->memory + num);
  }
  return num;
}

void psh(OPERATION_I) {
  cpu_pushstack(cpu, cpu_getloc(cpu, arg1));
}

void pop(OPERATION_I) {
  uint16_t val = cpu_popstack(cpu);
  *(cpu->memory + cpu_getloc(cpu, arg1)) = val;
}

void tst(OPERATION_I) {
  uint16_t a = cpu_getloc(cpu, arg1);
  uint16_t b = cpu_getloc(cpu, arg2);

  cpu->flags.zero = a == b;
  cpu->flags.sign = (a - b) & 0x7FFF;
}

void mov(OPERATION_I) {
  uint16_t from = cpu_getloc(cpu, arg1);
  *(cpu->memory + from) = cpu_getloc(cpu, arg2);
}
