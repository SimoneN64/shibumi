#pragma once
#include <registers.h>
#include <mem.h>

typedef struct {
  registers_t regs;
} cpu_t;

void init_cpu(cpu_t* cpu);
void step(cpu_t* cpu, mem_t* mem);
void handle_interrupt(cpu_t* cpu, mem_t* mem);
void exec(registers_t* regs, mem_t* mem, u32 instr);
void special(registers_t* regs, mem_t* mem, u32 instr);
void regimm(registers_t* regs, mem_t* mem, u32 instr);