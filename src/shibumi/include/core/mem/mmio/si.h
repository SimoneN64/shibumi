#pragma once
#include <intr.h>
#include <common.h>
#include <mi.h>

typedef struct {
  u32 status;
  bool dma_busy;
} si_t;

u32 si_read(mi_t* mi, si_t* si, u32 addr);
void si_write(mi_t* mi, registers_t* regs, si_t* si, u32 addr, u32 val);