#pragma once
#include <registers.h>
#include <mem.h>

void mtc0(registers_t* regs, u32 instr);
void mfc0(registers_t* regs, u32 instr);