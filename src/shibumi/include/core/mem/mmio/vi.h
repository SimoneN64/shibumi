#pragma once
#include <common.h>
#include <stdbool.h>

typedef struct mi_t mi_t;
typedef struct registers_t registers_t;

enum VI_FORMAT {
  blank = 0,
  reserved = 1,
  f5553 = 2,
  f8888 = 3
};

typedef union {
  struct {
    u8 format:2;
    unsigned serrate:1;
    unsigned:29;
  };

  u32 raw;
} status_t;

typedef struct {
  status_t status;
  u32 origin, width, current;
  u32 vsync, hsync, intr;
  int swaps;
  int num_halflines;
  int num_fields;
  int cycles_per_halfline;
} vi_t;

void init_vi(vi_t* vi);
u32 vi_read(vi_t* vi, u32 paddr);
void vi_write(mi_t* mi, registers_t* regs, vi_t* vi, u32 paddr, u32 val);
