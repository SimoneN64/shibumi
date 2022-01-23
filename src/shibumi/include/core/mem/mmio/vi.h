#pragma once
#include <common.h>
#include <stdbool.h>

enum VI_FORMAT {
  blank = 0,
  reserved = 1,
  f5553 = 2,
  f8888 = 3
};

typedef union {
  struct {
    unsigned format:2;
    unsigned remaining:30; // TODO: This is a stub
  };

  u32 raw;
} status_t;

typedef struct {
  status_t status;
  u32 origin, width, current;
  u32 vsync, hsync;
} vi_t;

void init_vi(vi_t* vi);
u32 vi_read(vi_t* vi, u32 paddr);
void vi_write(vi_t* vi, u32 paddr, u32 val);