#pragma once
#include <common.h>

typedef struct {
  u32 status, origin, width, current;
  u32 vsync, hsync;
} vi_t;

void init_vi(vi_t* vi);
u32 vi_read(vi_t* vi, u32 paddr);
void vi_write(vi_t* vi, u32 paddr, u32 val);