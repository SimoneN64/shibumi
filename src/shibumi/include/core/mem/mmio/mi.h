#pragma once
#include <common.h>
#include <stdbool.h>
#define MI_VERSION_REG 0x02020102

typedef struct registers_t registers_t;

typedef union {
  struct {
    unsigned sp:1;
    unsigned si:1;
    unsigned ai:1;
    unsigned vi:1;
    unsigned pi:1;
    unsigned dp:1;
    unsigned:26;
  };
  u32 raw;
} mi_intr_t;

typedef struct mi_t {
  u32 mi_mode;
  mi_intr_t mi_intr, mi_intr_mask;
} mi_t;

void init_mi(mi_t* mi);
void mi_write8(mi_t* mi, u8 val, u32 paddr);
u8 mi_read8(mi_t* mi, u32 paddr);
u32 mi_read(mi_t* mi, u32 paddr);
void mi_write(mi_t* mi, registers_t* regs, u32 paddr, u32 val);