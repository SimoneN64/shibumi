#pragma once
#include <intr.h>
#include <common.h>
#include <mi.h>

typedef struct {
  u32 status;
  bool dma_busy;
} si_t;

u32 si_read(si_t* si, mi_t* mi, u32 addr);
void si_write(si_t* si, mi_t* mi, registers_t* regs, u32 addr, u32 val);