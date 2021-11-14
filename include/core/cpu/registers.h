#pragma once
#include <cop0.h>
#include <cop1.h>
#include <stdbool.h>

static const char* regs_str[32] = {
  "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3",
  "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
  "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"
};

typedef struct registers_t {
  s64 gpr[32];
  cop1_t cp1;
  cop0_t cp0;
  s64 old_pc, pc, next_pc;
  s64 hi, lo;
  u32 instruction;
  bool LLBit;
} registers_t;

void init_registers(registers_t* regs);
void set_pc(registers_t* regs, s64 value);