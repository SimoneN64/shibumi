#pragma once
#include <registers.h>
#include <mem.h>

void mtc0(registers_t* regs, u32 instr);
void dmtc0(registers_t* regs, u32 instr);
void mfc0(registers_t* regs, u32 instr);
void dmfc0(registers_t* regs, u32 instr);
void eret(registers_t* regs);

void tlbr(registers_t* regs);
void tlbwi(registers_t* regs);
void tlbp(registers_t* regs);