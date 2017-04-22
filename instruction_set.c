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


struct PackedInstr decode(struct Cpu *cpu) {
  struct PackedInstr i;
  uint16_t *stream = cpu->memory + cpu->regs[rip]++;

  uint16_t opcode = *stream & 0x3FFF; // remove upper two bits
  int args = (*stream & 0xC000) >> 14;
  printf("Decoding instruction with ID: %" PRIu16 ", num args: %" PRIu16 ", full block: %" PRIu16 "\n", opcode, args, *stream);
  switch (args) {
    case 0:
      i.i = noArgs(opcode);
    case 1:
      i.i = oneArg(opcode);
      i.arg1 = *(cpu->memory + cpu->regs[rip]++);
      break;
    case 2:
      i.i = twoArgs(opcode);
      i.arg1 = *(cpu->memory + cpu->regs[rip]++);
      i.arg2 = *(cpu->memory + cpu->regs[rip]++);
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
    printf("[ERROR] Invalid 0 args instruction found, ID: %" PRIu16 "\n", opcode);
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
    case 3:
      return jeq;
    case 4:
      return jne;
    case 5:
      return jle;
    case 6:
      return jme;
    case 7:
      return ptc;
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 1 arg instruction found, ID: %" PRIu16 "\n", opcode);
    exit(1);
}

instruction twoArgs(uint16_t opcode) {
  switch (opcode) {
    case 0:
      return tst;
    case 1:
      return str;
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 2 args instruction found, ID: %" PRIu16 "\n", opcode);
    exit(1);
}

void halt(OPERATION_I){
  cpu->flags.halt = false;
}

void nop(OPERATION_I) {
  // yo it's a no op
}

void ret(OPERATION_I) {
  cpu_setreg(cpu, rip, cpu_popstack(cpu));
}

void call(OPERATION_I) {
  uint16_t loc = cpu_popstack(cpu); // get location to jump to
  uint16_t current = cpu->regs[rip];
  cpu_pushstack(cpu, ++current);
  cpu_setreg(cpu, rip, loc);
}

void jmp(OPERATION_I) {
  cpu_setreg(cpu, rip, arg1);
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

  cpu->flags.zero = !(a == b);
  cpu->flags.sign = a < b;
}

void str(OPERATION_I) {
  uint16_t from = cpu_getloc(cpu, arg1);
  *(cpu->memory + from) = cpu_getloc(cpu, arg2);
}

void jeq(OPERATION_I) {
  if (!cpu->flags.zero) cpu_setreg(cpu, rip, arg1);
}

void jne(OPERATION_I) {
  if (cpu->flags.zero) cpu_setreg(cpu, rip, arg1);
}

void jle(OPERATION_I) {  // less than
  if (cpu->flags.sign) cpu_setreg(cpu, rip, arg1);
}

void jme(OPERATION_I) {  // more than
  if (!cpu->flags.sign) cpu_setreg(cpu, rip, arg1);
}

void ptc(OPERATION_I) {
  printf("%c", (char) arg1);
}
