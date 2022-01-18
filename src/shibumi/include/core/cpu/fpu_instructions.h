#pragma once
#include <registers.h>
#include <mem.h>

void cfc1(registers_t* regs, u32 instr);
void ctc1(registers_t* regs, u32 instr);
void cvtls(registers_t* regs, u32 instr);
void cvtws(registers_t* regs, u32 instr);
void cvtds(registers_t* regs, u32 instr);
void cvtsw(registers_t* regs, u32 instr);
void cvtdw(registers_t* regs, u32 instr);
void cvtsd(registers_t* regs, u32 instr);
void cvtwd(registers_t* regs, u32 instr);
void cvtld(registers_t* regs, u32 instr);
void cvtdl(registers_t* regs, u32 instr);
void cvtsl(registers_t* regs, u32 instr);
void lwc1(registers_t* regs, mem_t* mem, u32 instr);
void swc1(registers_t* regs, mem_t* mem, u32 instr);
void ldc1(registers_t* regs, mem_t* mem, u32 instr);
void sdc1(registers_t* regs, mem_t* mem, u32 instr);
void mfc1(registers_t* regs, u32 instr);
void mtc1(registers_t* regs, u32 instr);
