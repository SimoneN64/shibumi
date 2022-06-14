#pragma once
#include <common.h>

typedef struct mem_t mem_t;

typedef union controller_t {
  struct {
    unsigned a:1;
    unsigned b:1;
    unsigned z:1;
    unsigned s:1;
    unsigned dU:1;
    unsigned dD:1;
    unsigned dL:1;
    unsigned dR:1;
    unsigned RST:1;
    unsigned:1;
    unsigned LT:1;
    unsigned RT:1;
    unsigned cU:1;
    unsigned cD:1;
    unsigned cL:1;
    unsigned cR:1;
    signed xaxis:8;
    signed yaxis:8;
  } PACKED;
  struct {
    u8 b1, b2;
    s8 b3, b4;
  } PACKED;
  u32 raw;
} controller_t;

static_assert(sizeof(controller_t) == 4, "Controller is not 4 bytes!");

void process_pif_commands(mem_t* mem);
