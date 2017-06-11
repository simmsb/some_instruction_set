#include "cpu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_TIME
#include <sys/time.h>
#include <time.h>
#endif

#define NANO_SECOND pow(10, 9)
#define DEFAULT_SIZE 1 << 16 // 2^16 bytes of ram

void run(struct Cpu *);

struct Cpu *init_cpu(int mem_size) {
  struct Cpu *cpu = malloc(sizeof(struct Cpu));
  cpu->memory = calloc(mem_size, sizeof(uint16_t));
  cpu->flags.halt = true;
  cpu->regs[rip] = 0;
  cpu->regs[esp] = mem_size;
  cpu->ticks = 0;
  cpu->interrupts = NULL;
  return cpu;
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
  uint16_t *prog = calloc(length / 4, sizeof(uint16_t));
  for (int i = 0; i < length / 4; i++) {
    for (int k = 0; k < 4; k++) {
      prog[i] += pow(16, 3 - k) * hex_value(hex[(i * 4) + k]);
    }
  }
  return prog;
}

void cpu_setreg(struct Cpu *cpu, uint16_t reg, uint16_t value) {
#if DEBUG_MOV
  printf("Setting register %" PRIu16 " to %" PRIu16 "\n", reg, value);
#endif
  cpu->regs[reg] = value;
}

uint16_t cpu_getloc(struct Cpu *cpu, uint16_t location) {
  uint16_t num;
  // bXY00000000000000
  // If X is set, Dereference
  // If Y is set, get value of register
  if (location & 1 << 14) {       // if register
    int regnum = location & 0xFF; // extract lower bits
    num = cpu->regs[regnum];
#if DEBUG_MOV
    printf("Getting value %" PRIu16 "from register %d\n", num, regnum);
#endif
  } else { // abs
    num = location & 0x3FFF;
  }
  return (location & 1 << 15) ? cpu->memory[num] : num;
}

void cpu_setloc(struct Cpu *cpu, uint16_t location, uint16_t data) {
#if DEBUG_MOV
  printf("Setting location %" PRIu16 " to %" PRIu16 "\n", location, data);
#endif
  if (location & 1 << 15)
    cpu->memory[cpu_getloc(cpu, location & 0x3FFF)] = data;
  else {
    if (location & 1 << 14)
      cpu_setreg(cpu, location & 0x1FFF, data);
    else
      cpu->memory[location & 0x1FFF] = data;
  }
}

inline uint16_t cpu_popstack(struct Cpu *cpu) {
  return cpu->memory[cpu->regs[esp]++]; // get value, move up
}

inline void cpu_pushstack(struct Cpu *cpu, uint16_t val) {
  cpu->memory[--cpu->regs[esp]] = val; // move down, set value
}

// print `Wew\n` -> 0x4007005740070065400700774007000A0003

void schedule(struct Cpu *cpu, struct Interrupt *i) {
  push_node(&cpu->interrupts, i);
}

void deschedule(struct Cpu *cpu,
                Node *node) { // might want to do more stuff sometime
  free_node(&cpu->interrupts, node);
}

void check_interrupts(struct Cpu *cpu) {
  Node *lst = cpu->interrupts;
  while (lst != NULL) {
    if (lst->irq->when < cpu->ticks) {
      lst->irq->callback(cpu, lst->irq->data);
      Node *next = lst->next; // save next                      v
      deschedule(cpu,
                 lst); // free node: | prev | lst | next | -> | prev | next |
      lst = next;      // move next
    } else { // only increment here, since free will pull the next node along
             // for us
      lst = lst->next;
    }
  }
}

void free_node(Node **head, Node *node) {
  if (*head == node)
    *head = node->next;

  if (node->next != NULL)
    node->next->prev = node->prev;

  if (node->prev != NULL)
    node->prev->next = node->next;

  free(node->irq);
  free(node);
}

void push_node(Node **head, struct Interrupt *irq) {
  Node *ptr = malloc(sizeof(Node));
  ptr->next = *head;
  ptr->irq = irq;
  if (*head != NULL)
    (*head)->prev = ptr;
  *head = ptr;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("One argument expected. USAGE: %s <program>\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct Cpu *cpu = init_cpu(DEFAULT_SIZE);
  uint16_t *program = parse_hex(argv[1]);

  memcpy(cpu->memory, program, sizeof(uint16_t) * strlen(argv[1]) / 4);
  free(program);

#ifdef DEBUG_TIME
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
#endif

  run(cpu);

#ifdef DEBUG_TIME
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("CPU shutting down, completed %" PRIu64 " cycles in %f s\n",
         cpu->ticks,
         (end.tv_sec - start.tv_sec) +
             (end.tv_nsec - start.tv_nsec) / NANO_SECOND);
#endif

  free(cpu->memory);
  free(cpu);
  return 0;
}

void run(struct Cpu *cpu) {
  uint16_t instr;
  while (cpu->flags.halt) {
    instr = cpu->memory[cpu->regs[rip]++];
    switch (instr) {
    case 0x0000: {
      break;
    } // nop
    case 0x0001: {
      cpu_setreg(cpu, rip, cpu_popstack(cpu));
      break;
    } // ret

    case 0x0002: {
      uint16_t loc = cpu_popstack(cpu); // get location to jump to
      uint16_t current = cpu->regs[rip];
      cpu_pushstack(cpu, ++current);
      cpu_setreg(cpu, rip, loc);
      break;
    } // call

    case 0x0003: {
      cpu->flags.halt = false;
      break;
    } // halt

    case 1 << 14: {
      cpu_setreg(cpu, rip, cpu->memory[cpu->regs[rip]++]);
      break;
    } // jmp

    case 0x4001: {
      cpu_pushstack(cpu, cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]));
      break;
    } // psh

    case 0x4002: {
      uint16_t val = cpu_popstack(cpu);
      cpu_setloc(cpu, cpu->memory[cpu->regs[rip]++], val);
      break;
    } // pop

    case 0x4003: {
      uint16_t loc = cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]);
      if (cpu->flags.zero)
        cpu_setreg(cpu, rip, loc);
      break;
    } // jeq

    case 0x4004: {
      uint16_t loc = cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]);
      if (!cpu->flags.zero)
        cpu_setreg(cpu, rip, loc);
      break;
    } // jne

    case 0x4005: {
      uint16_t loc = cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]);
      if (cpu->flags.sign)
        cpu_setreg(cpu, rip, loc);
      break;
    } // jle

    case 0x4006: {
      if (!cpu->flags.sign)
        cpu_setreg(cpu, rip, cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]));
      break;
    } // jme

    case 0x4007: {
      printf("%c", (char)cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]));
      break;
    } // ptc

    case 1 << 15: {
      uint16_t a = cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]);
      uint16_t b = cpu_getloc(cpu, cpu->memory[cpu->regs[rip]++]);
#if DEBUG_TST
      printf("a = %" PRIu16 ", b = %" PRIu16 "\n", a, b);
#endif

      cpu->flags.zero = a == b;
      cpu->flags.sign = a < b;
#if DEBUG_TST
      printf("zero flag is: %d\nsign flag is %d\n", cpu->flags.zero,
             cpu->flags.sign);
#endif
      break;
    } // tst

    case 0x8001: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu_setloc(cpu, arg1, cpu_getloc(cpu, arg2));
      break;
    } // mov

    case 0x8002: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) + cpu_getloc(cpu, arg2));
      break;
    } // add

    case 0x8003: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) - cpu_getloc(cpu, arg2));
      break;
    } // sub

    case 0x8004: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) * cpu_getloc(cpu, arg2));
      break;
    } // mul

    case 0x8005: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) / cpu_getloc(cpu, arg2));
      break;
    } // divn

    case 0x8006: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu_setreg(cpu, acc, cpu_getloc(cpu, arg1) % cpu_getloc(cpu, arg2));
      break;
    } // rem

    case 0x8007: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      uint16_t lower = cpu_getloc(cpu, arg2);
      uint16_t upper = cpu_getloc(cpu, arg1);

      float value = (float)(upper << 16) + lower;
      cpu->fregs[aaa] = value;
      break;
    } // flc

    case 0x8008: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      float value = cpu->fregs[aaa];
      uint32_t c = (uint32_t)value;

      uint16_t upper = (c >> 16) & 0xFFFF;
      uint16_t lower = c & 0xFFFF;

      cpu_setloc(cpu, arg1, upper);
      cpu_setloc(cpu, arg2, lower);
      break;
    } // clf

    case 0x8009: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      float value = cpu->fregs[aaa];
      uint32_t c = *(uint32_t *)&value;

      uint16_t upper = (c >> 16) & 0xFFFF;
      uint16_t lower = c & 0xFFFF;

      cpu_setloc(cpu, arg1, upper);
      cpu_setloc(cpu, arg2, lower);
      break;
    } // stf

    case 0x800A: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      uint16_t lower = cpu_getloc(cpu, arg2);
      uint16_t upper = cpu_getloc(cpu, arg1);

      uint32_t value = (upper << 16) + lower;
      float fval = *(float *)&value;
      cpu->fregs[aaa] = fval;
      break;
    } // ldf

    case 0x800B: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu->fregs[cpu_getloc(cpu, arg1)] = cpu->regs[cpu_getloc(cpu, arg2)];
      break;
    } // mvf

    case 0x800C: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu->fregs[acc] =
          cpu->fregs[cpu_getloc(cpu, arg1)] + cpu->fregs[cpu_getloc(cpu, arg2)];
      break;
    } // fad

    case 0x800D: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu->fregs[acc] =
          cpu->fregs[cpu_getloc(cpu, arg1)] - cpu->fregs[cpu_getloc(cpu, arg2)];
      break;
    } // fsb

    case 0x800E: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu->fregs[acc] =
          cpu->fregs[cpu_getloc(cpu, arg1)] * cpu->fregs[cpu_getloc(cpu, arg2)];
      break;
    } // fmu

    case 0x800F: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      cpu->fregs[acc] =
          cpu->fregs[cpu_getloc(cpu, arg1)] / cpu->fregs[cpu_getloc(cpu, arg2)];
      break;
    } // fdv

    case 0x8010: {
      uint16_t arg1 = cpu->memory[cpu->regs[rip]++];
      uint16_t arg2 = cpu->memory[cpu->regs[rip]++];
      uint16_t time_ = cpu_getloc(cpu, arg2);
      void (*callback)(struct Cpu *, uint16_t *);
      int args;
      int irq_num = cpu_getloc(cpu, arg1);

      switch (irq_num) {
      case 0:
        callback = timed_jump;
        args = 1;
      }

      struct Interrupt *irq =
          malloc(sizeof(struct Interrupt *) + (args * sizeof(uint16_t) - 1));
      irq->when = cpu->ticks + time_;
      irq->callback = callback;

      switch (irq_num) {
      case 0:
        irq->data[0] = cpu->regs[aaa];
      }
      schedule(cpu, irq);
      break;
    } // irq

    default:
      goto ERROR;
    }
  }

  return;

ERROR:
  printf("Couldn't decode instruction, exiting!, RIP = %" PRIu16 "\n",
         cpu->regs[rip]);
  exit(1);
}

void timed_jump(struct Cpu *cpu, uint16_t *data) { cpu->regs[rip] = data[0]; }
