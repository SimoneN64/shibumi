#pragma once
#include <registers.h>
#include <mem.h>

typedef enum {
  T,   UN,   EQ,  UEQ,
  OLT, ULT,  OLE, ULE,
  SF,  NGLE, SEQ, NGL,
  LT,  NGE,  LE,  NGT,
  F,   OR,   NEQ, OLG,
  UGE, OGE,  UGT, OGT,
  ST,  GLE,  SNE, GL,
  NLT, GE,   NLE, GT
} comp_conds;

void absd(registers_t* regs, u32 instr);
void abss(registers_t* regs, u32 instr);
void absw(registers_t* regs, u32 instr);
void absl(registers_t* regs, u32 instr);
void adds(registers_t* regs, u32 instr);
void addd(registers_t* regs, u32 instr);
void subs(registers_t* regs, u32 instr);
void subd(registers_t* regs, u32 instr);
void subw(registers_t* regs, u32 instr);
void subl(registers_t* regs, u32 instr);
void ceills(registers_t* regs, u32 instr);
void ceilws(registers_t* regs, u32 instr);
void ceilld(registers_t* regs, u32 instr);
void ceilwd(registers_t* regs, u32 instr);
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
void ccondd(registers_t* regs, u32 instr, comp_conds cond);
void cconds(registers_t* regs, u32 instr, comp_conds cond);
void divs(registers_t* regs, u32 instr);
void divd(registers_t* regs, u32 instr);
void muls(registers_t* regs, u32 instr);
void muld(registers_t* regs, u32 instr);
void mulw(registers_t* regs, u32 instr);
void mull(registers_t* regs, u32 instr);
void movs(registers_t* regs, u32 instr);
void movd(registers_t* regs, u32 instr);
void movw(registers_t* regs, u32 instr);
void movl(registers_t* regs, u32 instr);
void lwc1(registers_t* regs, mem_t* mem, u32 instr);
void swc1(registers_t* regs, mem_t* mem, u32 instr);
void ldc1(registers_t* regs, mem_t* mem, u32 instr);
void mfc1(registers_t* regs, u32 instr);
void mtc1(registers_t* regs, u32 instr);
void sdc1(registers_t* regs, mem_t* mem, u32 instr);
void truncws(registers_t* regs, u32 instr);
void truncwd(registers_t* regs, u32 instr);
void truncls(registers_t* regs, u32 instr);
void truncld(registers_t* regs, u32 instr);
