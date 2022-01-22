#pragma once
#include <registers.h>
#include <mem.h>

typedef struct cpu_t cpu_t;

void mtc0(cpu_t* cpu, mem_t* mem, u32 instr);
void mfc0(registers_t* regs, u32 instr);
void eret(registers_t* regs);