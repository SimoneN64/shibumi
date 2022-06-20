#pragma once
#include <sp_status.h>
#include <mi.h>

typedef struct mem_t mem_t;
typedef struct registers_t registers_t;

typedef union {
  struct {
    unsigned len:12;
    unsigned count:8;
    unsigned skip:12;
  };
  u32 raw;
} sp_dma_len_t;

typedef union {
  struct {
    unsigned address:12;
    unsigned bank:1;
    unsigned: 19;
  };
  u32 raw;
} sp_dma_sp_addr_t;

typedef union {
  struct {
    unsigned address:24;
    unsigned:8;
  };
  u32 raw;
} sp_dma_dram_addr_t;

typedef struct {
  sp_status_t sp_status;
  u16 old_pc, pc, next_pc;
  sp_dma_sp_addr_t sp_dma_sp_addr;
  sp_dma_dram_addr_t sp_dma_dram_addr;
  sp_dma_sp_addr_t shadow_sp_dma_sp_addr;
  sp_dma_dram_addr_t shadow_sp_dma_dram_addr;
  sp_dma_len_t sp_dma_rdlen, sp_dma_wrlen;
  u8 dmem[DMEM_SIZE], imem[IMEM_SIZE];
} rsp_t;

void init_rsp(rsp_t* rsp);
void step_rsp(rsp_t* rsp);
u32 sp_read(rsp_t* rsp, u32 addr);
void sp_write(rsp_t* rsp, mem_t* mem, registers_t* regs, u32 addr, u32 value);
