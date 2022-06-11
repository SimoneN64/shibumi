#pragma once
#include <intr.h>
#include <common.h>
#include <mi.h>

typedef union {
  struct {
    unsigned dma_busy:1;
    unsigned io_busy:1;
    unsigned reserved:1;
    unsigned dma_err:1;
    unsigned:8;
    unsigned intr:1;
  };
  u32 raw;
} si_status_t;

typedef struct {
  si_status_t status;
} si_t;

void init_si(si_t* si);
u32 si_read(si_t* si, mi_t* mi, u32 addr);
void si_write(si_t* si, mi_t* mi, registers_t* regs, u32 addr, u32 val);
