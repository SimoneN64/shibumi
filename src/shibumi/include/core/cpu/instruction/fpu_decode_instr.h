#pragma once
#include <fpu_instructions.h>

typedef struct cpu_t cpu_t;

void fpu_decode(cpu_t* cpu, u32 instr);