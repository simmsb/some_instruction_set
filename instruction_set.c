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

void decode(struct Cpu *cpu, struct PackedInstr *i) {
  #ifdef DEBUG
    printf("cpu->regs[rip] = %" PRIu16 "\n", cpu->regs[rip]);
  #endif

  uint16_t instruction = cpu->memory[cpu->regs[rip]++];

  uint16_t opcode = instruction & 0x3FFF; // remove upper two bits
  int args = (instruction & 0xC000) >> 14;

  #ifdef DEBUG
    printf("Decoding instruction of opcode %" PRIu16 " with num args %d\n", opcode, args);
  #endif

  switch (args) {
    case 0:
      i->i = noArgs(opcode);
      break;
    case 1:
      i->i = oneArg(opcode);
      i->arg1 = cpu->memory[cpu->regs[rip]++];
      break;
    case 2:
      i->i = twoArgs(opcode);
      i->arg1 = cpu->memory[cpu->regs[rip]++];
      i->arg2 = cpu->memory[cpu->regs[rip]++];
      break;
    default:
      goto ERROR;
  }

  return;

  ERROR:
    printf("[ERROR] failed to decode instruction, [RIP] = %" PRIu16 " instruction = %" PRIu16 " n args = %d\n", cpu->regs[rip], opcode, args);
    exit(1);
}

instruction noArgs(uint16_t opcode) {
  switch (opcode) {
    case 0: return nop;
    case 1: return ret;
    case 2: return call;
    case 3: return halt;
    default: goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 0 args instruction found, ID: %" PRIu16 "\n", opcode);
    exit(1);
}

instruction oneArg(uint16_t opcode) {
  switch (opcode) {
    case 0: return jmp;
    case 1: return psh;
    case 2: return pop;
    case 3: return jeq;
    case 4: return jne;
    case 5: return jle;
    case 6: return jme;
    case 7: return ptc;
    default: goto ERROR;
  }

  ERROR:
    printf("[ERROR] Invalid 1 arg instruction found, ID: %" PRIu16 "\n", opcode);
    exit(1);
}

instruction twoArgs(uint16_t opcode) {
  switch (opcode) {
    case 0: return tst;
    case 1: return mov;
    case 2: return add;
    case 3: return sub;
    case 4: return mul;
    case 5: return divn;
    case 6: return rem;
    case 7: return flc;
    case 8: return clf;
    case 9: return stf;
    case 10: return ldf;
    case 11: return mvf;
    case 12: return fad;
    case 13: return fsb;
    case 14: return fmu;
    case 15: return fdv;
    case 16: return irq;
    default: goto ERROR;
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
  printf("call\n");
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
  cpu_setloc(cpu, arg1, val);
}


void jeq(OPERATION_I) {
  if (!cpu->flags.zero) cpu_setreg(cpu, rip, cpu_getloc(cpu, arg1));
}

void jne(OPERATION_I) {
  if (cpu->flags.zero) cpu_setreg(cpu, rip, cpu_getloc(cpu, arg1));
}

void jle(OPERATION_I) {  // less than
  if (cpu->flags.sign) cpu_setreg(cpu, rip, cpu_getloc(cpu, arg1));
}

void jme(OPERATION_I) {  // more than
  if (!cpu->flags.sign) cpu_setreg(cpu, rip, cpu_getloc(cpu, arg1));
}

void ptc(OPERATION_I) {
  printf("%c", (char) cpu_getloc(cpu, arg1));
}

void tst(OPERATION_I) {
  uint16_t a = cpu_getloc(cpu, arg1);
  uint16_t b = cpu_getloc(cpu, arg2);
  #if DEBUG_TST
    printf("a = %" PRIu16 ", b = %" PRIu16 "\n", a, b);
  #endif

  cpu->flags.zero = a == b;
  cpu->flags.sign = a < b;
  #if DEBUG_TST
    printf("zero flag is: %d\nsign flag is %d\n", cpu->flags.zero, cpu->flags.sign);
  #endif
}

void mov(OPERATION_I) {
  cpu_setloc(cpu, arg1, cpu_getloc(cpu, arg2));
}

void irq(OPERATION_I) { // TODO: Think of a better way for this
  uint16_t time_ = cpu_getloc(cpu, arg2);
  void (*callback)(struct Cpu *, uint16_t *);
  int args;
  int irq_num = cpu_getloc(cpu, arg1);

  switch (irq_num) {
    case 0:
      callback = timed_jump;
      args = 1;
      break;
  }

  struct Interrupt *irq = malloc(sizeof(struct Interrupt *) + (args * sizeof(uint16_t) - 1));
  irq->when = cpu->ticks + time_;
  irq->callback = callback;

  switch (irq_num) {
    case 0:
      irq->data[0] = cpu->regs[aaa];
  }
  schedule(cpu, irq);
}

void add(OPERATION_I) {
  cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) + cpu_getloc(cpu, arg2));
}
void sub(OPERATION_I) {
  cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) - cpu_getloc(cpu, arg2));
}
void mul(OPERATION_I) {
  cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) * cpu_getloc(cpu, arg2));
}

void divn(OPERATION_I) {
  cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) / cpu_getloc(cpu, arg2));
}
void rem(OPERATION_I) {
  cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) % cpu_getloc(cpu, arg2));
}

void flc(OPERATION_I) { // convert arg1 and arg2 into float and store in float reg aaa
  uint16_t lower = cpu_getloc(cpu, arg2);
  uint16_t upper = cpu_getloc(cpu, arg1);

  float value = (float) (upper << 16) + lower;
  cpu->fregs[aaa ] = value;
}

void clf(OPERATION_I) { // convert float register aaa into two ints and store in arg1 and arg2
  float value = cpu->fregs[aaa];
  uint32_t c = (uint32_t) value;

  uint16_t upper = (c >> 16) & 0xFFFF;
  uint16_t lower = c & 0xFFFF;

  cpu_setloc(cpu, arg1, upper);
  cpu_setloc(cpu, arg2, lower);
}

void stf(OPERATION_I) { // store upper 2 bytes of float in arg1, lower 2 bytes in arg2 (no cast)
  float value = cpu->fregs[aaa];
  uint32_t c = *(uint32_t *) &value;

  uint16_t upper = (c >> 16) & 0xFFFF;
  uint16_t lower = c & 0xFFFF;

  cpu_setloc(cpu, arg1, upper);
  cpu_setloc(cpu, arg1, lower);
}

void ldf(OPERATION_I) { // load a float from memory into float register aaa (no cast)
  uint16_t lower = cpu_getloc(cpu, arg2);
  uint16_t upper = cpu_getloc(cpu, arg1);

  uint32_t value = (upper << 16) + lower;
  float fval = *(float *) &value;
  cpu->fregs[aaa] = fval;
}

void mvf(OPERATION_I) {
  cpu->fregs[cpu_getloc(cpu, arg1)] = cpu->regs[cpu_getloc(cpu, arg2)];
}

void fad(OPERATION_I) {
  cpu->fregs[acc] = cpu->fregs[cpu_getloc(cpu, arg1)] + cpu->fregs[cpu_getloc(cpu, arg2)];
}

void fsb(OPERATION_I) {
  cpu->fregs[acc] = cpu->fregs[cpu_getloc(cpu, arg1)] - cpu->fregs[cpu_getloc(cpu, arg2)];
}

void fmu(OPERATION_I) {
  cpu->fregs[acc] = cpu->fregs[cpu_getloc(cpu, arg1)] * cpu->fregs[cpu_getloc(cpu, arg2)];
}

void fdv(OPERATION_I) {
  cpu->fregs[acc] = cpu->fregs[cpu_getloc(cpu, arg1)] / cpu->fregs[cpu_getloc(cpu, arg2)];
}

void timed_jump(struct Cpu *cpu, uint16_t *data) {
  cpu->regs[rip] = data[0];
}
