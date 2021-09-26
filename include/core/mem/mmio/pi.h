#pragma once
#include <common.h>
#include <intr.h>

typedef struct mem_t mem_t;
typedef struct registers_t registers_t;

typedef struct {
  u32 dram_addr, cart_addr;
  u32 rd_len, wr_len;
  u32 status;
} pi_t;

void init_pi(pi_t* pi);
u32 pi_read(pi_t* pi, u32 paddr);
void pi_write(mem_t* mem, registers_t* regs, u32 paddr, u32 val);