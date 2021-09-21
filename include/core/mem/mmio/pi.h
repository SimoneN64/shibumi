#pragma once
#include <common.h>

typedef struct mem_t mem_t;

typedef union {
  struct {
    unsigned reset_controller:1;
    unsigned clear_intr:1;
    unsigned error:1;
    unsigned:29;
  };
  u32 raw;
} status_t;

typedef struct {
  u32 dram_addr, cart_addr;
  u32 rd_len, wr_len;
  status_t status;
} pi_t;

void init_pi(pi_t* pi);
u32 pi_read(pi_t* pi, u32 paddr);
void pi_write(mem_t* mem, u32 paddr, u32 val);