#pragma once
#include <registers.h>
#include <mem.h>

typedef struct cpu_t {
  registers_t regs;
  bool in_delay_slot;
} cpu_t;

typedef enum exception_code_t {
  Int,
  Mod,
  TLBL,
  TLBS,
  AdEL,
  AdES,
  IBE,
  DBE,
  Sys,
  Bp,
  RI,
  CpU,
  Ov,
  Tr,
  FPE = 15,
  WATCH = 23
} exception_code_t;

void init_cpu(cpu_t* cpu);
void step(cpu_t* cpu, mem_t* mem);
