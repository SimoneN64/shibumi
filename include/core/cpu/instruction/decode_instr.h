#pragma once
#include <instructions.h>

void exec(registers_t* regs, mem_t* mem, u32 instr);
void special(registers_t* regs, mem_t* mem, u32 instr);
void regimm(registers_t* regs, mem_t* mem, u32 instr);