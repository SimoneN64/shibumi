#pragma once
#include <common.h>

typedef struct mem_t mem_t;

typedef union controller_t {
  struct {
    u8 b1, b2;
    s8 b3, b4;
  } PACKED;
  u32 raw;
} controller_t;

static_assert(sizeof(controller_t) == 4, "Controller is not 4 bytes!");

void process_pif_commands(mem_t* mem);
