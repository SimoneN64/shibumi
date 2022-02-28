#pragma once
#include <registers.h>
#include <mem.h>

typedef struct cpu_t {
  registers_t regs;
} cpu_t;

void init_cpu(cpu_t* cpu);
void step(cpu_t* cpu, mem_t* mem);
void handle_exception(cpu_t* cpu, mem_t* mem);