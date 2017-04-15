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



void nop(struct Cpu*, unsigned int);



struct PackedInstr decode(char **stream){

  char opcode = 0;
  unsigned int operands = 0;

  opcode = **stream & 0x3F; // remove upper two bits
  switch ((**stream & 0xC0) >> 6) { // extract top 2 bits then downshift
    case 2:
      operands |= ((int) *++*stream) << 16; // get first arg, place at top
    case 1:
      operands |= (int) *++*stream; // get (first or second arg), place at bottom (if this is a single arg instruction, arg is in the lower byte)
      break;
  }

  struct PackedInstr p;
  p.args = operands;

  switch (opcode) {
    case 0:
      p.i = nop;
  }

  return p;
}
