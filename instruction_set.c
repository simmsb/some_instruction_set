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
  i.n = (*stream & 0xC000) >> 14;
  switch (i.n) {
    case 0:
      i.i = noArgs(opcode);
      break;
    case 1:
      i.i = oneArg(opcode);
      i.arg1 = *++stream;
      cpu->regs.rip++;
      break;
    case 2:
      i.i = twoArgs(opcode);
      i.arg1 = *++stream;
      i.arg2 = *++stream;
      cpu->regs.rip += 2;
      break;
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
      return sjmp;
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
  size_t data = (size_t) cpu_popstack(cpu); // do this because memory is bytes and we need a 16 bit pointer
  cpu->instructions = (char *) ((upper << 8) + lower);
}

void call(OPERATION_I) {
  size_t address_upper = (size_t) cpu_popstack(cpu); // get location to jump to
  size_t address_lower = (size_t) cpu_popstack(cpu);

  size_t current_address = (size_t) cpu->instructions; // get return address
  size_t return_lower = current_address & 0xff;
  size_t return_upper = (current_address >> 8) & 0xff;

  cpu_pushstack(cpu, (char) return_upper);
  cpu_pushstack(cpu, (char) return_lower);

  cpu->instructions = (char *) ((address_upper << 8) + address_lower); // set instruction pointer
}

void sjmp(OPERATION_I) {
 // TODO: this
}
