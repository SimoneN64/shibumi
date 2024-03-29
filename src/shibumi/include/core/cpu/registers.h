#pragma once
#include <cop0.h>
#include <cop1.h>
#include <stdbool.h>

#define RD(x) (((x) >> 11) & 0x1F)
#define RT(x) (((x) >> 16) & 0x1F)
#define RS(x) (((x) >> 21) & 0x1F)
#define FD(x) (((x) >>  6) & 0x1F)
#define FT(x) RT(x)
#define FS(x) RD(x)
#define base(x) RS(x)

typedef struct registers_t {
  s64 gpr[32];
  cop1_t cp1;
  cop0_t cp0;
  s64 old_pc, pc, next_pc;
  s64 hi, lo;
  bool LLBit;
  bool prev_delay_slot, delay_slot;
} registers_t;

void init_registers(registers_t* regs);
void set_pc(registers_t* regs, s64 value);
