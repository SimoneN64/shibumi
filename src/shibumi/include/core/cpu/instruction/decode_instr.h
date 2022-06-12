#pragma once
#include <instructions.h>

typedef struct cpu_t cpu_t;

void exec(cpu_t* cpu, mem_t* mem, u32 instr);
void special(cpu_t* cpu, u32 instr);
void regimm(cpu_t* cpu, u32 instr);