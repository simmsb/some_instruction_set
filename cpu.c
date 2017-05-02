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
  cpu->regs[rip] = 0;
  cpu->regs[esp] = mem_size;
  cpu->ticks = 0;
  cpu->interrupts = NULL;
  return cpu;
}

void load_program(struct Cpu *cpu, uint16_t *restrict program, int length) {
  for (int i=0; i < length; i++) {
    #ifdef DEBUG
      printf("Inserting data %" PRIu16 " at position %d in memory\n", program[i], i);
    #endif
    cpu->memory[i] = program[i];
  }
  #ifdef DEBUG
    printf("Memory at position 0 is %" PRIu16 "\n", cpu->memory[0]);
  #endif
}

void run_cmd(struct Cpu *cpu) {
  struct PackedInstr instr = decode(cpu);
  instr.i(cpu, instr.arg1, instr.arg2);
}

void run(struct Cpu *cpu) {
  #ifdef DEBUG
    puts("Starting cpu");
  #endif

  while (cpu->flags.halt) {
    #ifdef DEBUG
      printf("Cpu on cycle: %" PRIu64 ". \nreg state:\n", cpu->ticks);
    #endif
    run_cmd(cpu);
    check_interrupts(cpu);
    cpu->ticks++;
  }

  #ifdef DEBUG
    puts("Closing cpu");
  #endif
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

  load_program(cpu, program, strlen(argv[1])/4);
  free(program);
  run(cpu);
  free(cpu->memory);
  free(cpu);
  return 0;
}

void cpu_setreg(struct Cpu *cpu, uint16_t reg, uint16_t value) {
  cpu->regs[reg] = value;
}

uint16_t cpu_getloc(struct Cpu *cpu, uint16_t location) {
  uint16_t num;
  // bXY00000000000000
  // If X is set, Dereference
  // If Y is set, get value of register
  if (location & 0x4000) { // if register
    int regnum = ((location & 0x3C00) >> 10) & 0xF;
    num = cpu->regs[regnum];
  } else { // abs
    num = location & 0x3FFF;
  }
  return (location & 0x8000)?*(cpu->memory + num):num;
}

void cpu_setloc(struct Cpu *cpu, uint16_t location, uint16_t data) {
  if (location & 0x8000) *(cpu->memory + cpu_getloc(cpu, location & 0x3FFF)) = data;
  else {
    if (location & 0x4000)
      cpu_setreg(cpu, location & 0x1FFF, data);
    else *(cpu->memory + (location & 0x1FFF)) = data;
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

void deschedule(struct Cpu *cpu, Node *node) { // might want to do more stuff sometime
  free_node(&cpu->interrupts, node);
}

void check_interrupts(struct Cpu *cpu) {
  Node *lst = cpu->interrupts;
  while (lst != NULL) {
    if (lst->irq->when < cpu->ticks) {
      lst->irq->callback(cpu, lst->irq->data);
      Node *next = lst->next; // save next                      v
      deschedule(cpu, lst); // free node: | prev | lst | next | -> | prev | next |
      lst = next; // move next
    } else { // only increment here, since free will pull the next node along for us
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
