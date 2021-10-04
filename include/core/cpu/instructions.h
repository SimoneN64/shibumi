#pragma once
#include <registers.h>
#include <mem.h>

void mtcz(registers_t* regs, u32 instr, u8 index);
void mfcz(registers_t* regs, u32 instr, u8 index);
void cfcz(registers_t* regs, u32 instr, u8 index);
void ctcz(registers_t* regs, u32 instr, u8 index);
void lui(registers_t* regs, u32 instr);
void addiu(registers_t* regs, u32 instr);
void daddiu(registers_t* regs, u32 instr);
void branch(registers_t* regs, bool cond, s64 address);
void branch_likely(registers_t* regs, bool cond, s64 address);
void b(registers_t* regs, u32 instr, bool cond);
void bl(registers_t* regs, u32 instr, bool cond);
void lbu(mem_t* mem, registers_t* regs, u32 instr);
void lh(mem_t* mem, registers_t* regs, u32 instr);
void lw(mem_t* mem, registers_t* regs, u32 instr);
void lwu(mem_t* mem, registers_t* regs, u32 instr);
void sb(mem_t* mem, registers_t* regs, u32 instr);
void sh(mem_t* mem, registers_t* regs, u32 instr);
void sw(mem_t* mem, registers_t* regs, u32 instr);
void ori(registers_t* regs, u32 instr);
void or_(registers_t* regs, u32 instr);
void jal(registers_t* regs, u32 instr);
void jalr(registers_t* regs, u32 instr);
void slti(registers_t* regs, u32 instr);
void slt(registers_t* regs, u32 instr);
void sltu(registers_t* regs, u32 instr);
void srlv(registers_t* regs, u32 instr);
void sllv(registers_t* regs, u32 instr);
void xori(registers_t* regs, u32 instr);
void xor_(registers_t* regs, u32 instr);
void andi(registers_t* regs, u32 instr);
void and_(registers_t* regs, u32 instr);
void sll(registers_t* regs, u32 instr);
void srl(registers_t* regs, u32 instr);
void jr(registers_t* regs, u32 instr);
void j(registers_t* regs, u32 instr);
void subu(registers_t* regs, u32 instr);
void multu(registers_t* regs, u32 instr);
void mflo(registers_t* regs, u32 instr);
void mfhi(registers_t* regs, u32 instr);
void addu(registers_t* regs, u32 instr);
void add(registers_t* regs, u32 instr);