#pragma once
#include <common.h>

typedef struct {
  s64 gpr[32];
  s64 cp0[32];
  s64 old_pc, pc, next_pc;
  s64 hi, lo;
} registers_t;

#define ZERO(x) do { x->gpr[0] = 0; } while(0)

void init_registers(registers_t* regs);
void set_pc(registers_t* regs, s64 value);