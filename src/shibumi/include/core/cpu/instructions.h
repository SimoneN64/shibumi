#pragma once
#include <registers.h>
#include <mem.h>
#include <cop0_instructions.h>

void add(registers_t* regs, u32 instr);
void addu(registers_t* regs, u32 instr);
void addiu(registers_t* regs, u32 instr);
void andi(registers_t* regs, u32 instr);
void and_(registers_t* regs, u32 instr);
void branch(registers_t* regs, bool cond, s64 address);
void branch_likely(registers_t* regs, bool cond, s64 address);
void b(registers_t* regs, u32 instr, bool cond);
void blink(registers_t* regs, u32 instr, bool cond);
void bl(registers_t* regs, u32 instr, bool cond);
void bllink(registers_t* regs, u32 instr, bool cond);
void daddu(registers_t* regs, u32 instr);
void daddiu(registers_t* regs, u32 instr);
void ddiv(registers_t* regs, u32 instr);
void ddivu(registers_t* regs, u32 instr);
void div_(registers_t* regs, u32 instr);
void divu(registers_t* regs, u32 instr);
void dmult(registers_t* regs, u32 instr);
void dmultu(registers_t* regs, u32 instr);
void dsll(registers_t* regs, u32 instr);
void dsllv(registers_t* regs, u32 instr);
void dsll32(registers_t* regs, u32 instr);
void dsra(registers_t* regs, u32 instr);
void dsrav(registers_t* regs, u32 instr);
void dsra32(registers_t* regs, u32 instr);
void dsrl(registers_t* regs, u32 instr);
void dsrlv(registers_t* regs, u32 instr);
void dsrl32(registers_t* regs, u32 instr);
void dsub(registers_t* regs, u32 instr);
void dsubu(registers_t* regs, u32 instr);
void j(registers_t* regs, u32 instr);
void jr(registers_t* regs, u32 instr);
void jal(registers_t* regs, u32 instr);
void jalr(registers_t* regs, u32 instr);
void lui(registers_t* regs, u32 instr);
void lbu(mem_t* mem, registers_t* regs, u32 instr);
void lb(mem_t* mem, registers_t* regs, u32 instr);
void ld(mem_t* mem, registers_t* regs, u32 instr);
void ldl(mem_t* mem, registers_t* regs, u32 instr);
void ldr(mem_t* mem, registers_t* regs, u32 instr);
void lh(mem_t* mem, registers_t* regs, u32 instr);
void lhu(mem_t* mem, registers_t* regs, u32 instr);
void ll(mem_t* mem, registers_t* regs, u32 instr);
void lld(mem_t* mem, registers_t* regs, u32 instr);
void lw(mem_t* mem, registers_t* regs, u32 instr);
void lwl(mem_t* mem, registers_t* regs, u32 instr);
void lwu(mem_t* mem, registers_t* regs, u32 instr);
void lwr(mem_t* mem, registers_t* regs, u32 instr);
void mfhi(registers_t* regs, u32 instr);
void mflo(registers_t* regs, u32 instr);
void mult(registers_t* regs, u32 instr);
void multu(registers_t* regs, u32 instr);
void mthi(registers_t* regs, u32 instr);
void mtlo(registers_t* regs, u32 instr);
void nor(registers_t* regs, u32 instr);
void sb(mem_t* mem, registers_t* regs, u32 instr);
void sc(mem_t* mem, registers_t* regs, u32 instr);
void scd(mem_t* mem, registers_t* regs, u32 instr);
void sd(mem_t* mem, registers_t* regs, u32 instr);
void sdl(mem_t* mem, registers_t* regs, u32 instr);
void sdr(mem_t* mem, registers_t* regs, u32 instr);
void sh(mem_t* mem, registers_t* regs, u32 instr);
void sw(mem_t* mem, registers_t* regs, u32 instr);
void swl(mem_t* mem, registers_t* regs, u32 instr);
void swr(mem_t* mem, registers_t* regs, u32 instr);
void slti(registers_t* regs, u32 instr);
void sltiu(registers_t* regs, u32 instr);
void slt(registers_t* regs, u32 instr);
void sltu(registers_t* regs, u32 instr);
void sll(registers_t* regs, u32 instr);
void sllv(registers_t* regs, u32 instr);
void sub(registers_t* regs, u32 instr);
void subu(registers_t* regs, u32 instr);
void sra(registers_t* regs, u32 instr);
void srav(registers_t* regs, u32 instr);
void srl(registers_t* regs, u32 instr);
void srlv(registers_t* regs, u32 instr);
void or_(registers_t* regs, u32 instr);
void ori(registers_t* regs, u32 instr);
void xor_(registers_t* regs, u32 instr);
void xori(registers_t* regs, u32 instr);