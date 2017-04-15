/*

each instruction is of 1 to 3 bytes long.

first two bits decide on length of instruction (00, 01, 10)

1 byte: [0 0] [0 0 0 0 0 0] (single opcodes only)
                 opcode

2 byte: [0 1] [0 0 0 0 0 0] [0 0 0 0 0 0 0 0]
                 opcode         operand 1

3 byte: [1 0] [0 0 0 0 0 0] [0 0 0 0 0 0 0 0] [0 0 0 0 0 0 0 0]
                 opcode         operand 1         operand 2

*/

#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>

#define OPERATION struct Cpu *, unsigned int
#define OPERATION_I struct Cpu *cpu, unsigned int args


void nop(OPERATION);
void ret(OPERATION);
void call(OPERATION);

instruction  noArgs(char);
instruction  oneArg(char);
instruction twoArgs(char);


struct PackedInstr decode(char **stream) {

  char opcode = 0;
  unsigned int operands = 0;

  opcode = **stream & 0x3F; // remove upper two bits
  int numargs = (**stream & 0xC0) >> 6;
  switch (numargs) { // extract top 2 bits then downshift
    case 2:
      operands |= ((int) *++*stream) << 8; // get first arg, place at top
    case 1:
      operands |= (int) *++*stream; // get (first or second arg), place at bottom (if this is a single arg instruction, arg is in the lower byte)
      break;
    default:
      goto ERROR;
  }

  (*stream)++;

  struct PackedInstr p;
  p.args = operands;

  switch (numargs) {
    case 0:
      p.i = noArgs(opcode);
      break;
    case 1:
      p.i = oneArg(opcode);
      break;
    case 2:
      p.i = twoArgs(opcode);
      break;
    default:
      goto ERROR;
  }

  return p;

  ERROR:
    exit(1);
}

instruction noArgs(char opcode) {
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

instruction oneArg(char opcode) {
  switch (opcode) {
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 1 arg instruction found, ID: %d\n", (int) opcode);
    exit(1);
}

instruction twoArgs(char opcode) {
  switch (opcode) {
    default:
      goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 2 args instruction found, ID: %d\n", (int) opcode);
    exit(1);
}

void nop(OPERATION_I) {
}

void ret(OPERATION_I) {

}

void call(OPERATION_I) {
}
