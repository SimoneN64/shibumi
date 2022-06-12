#pragma once
#include <common.h>
#include <intr.h>
#include <stdbool.h>

typedef struct mem_t mem_t;
typedef struct registers_t registers_t;

typedef struct ai_t {
  bool dma_enable;
  u16 dac_rate;
  u8 bitrate;
  int dma_count;
  u32 dma_length[2];
  u32 dma_address[2];
  bool dma_address_carry;
  int cycles;

  struct {
    u32 frequency;
    u32 period;
    u32 precision;
  } dac;
} ai_t;

void init_ai(ai_t* ai);
u32 ai_read(ai_t* ai, u32 paddr);
void ai_write(mem_t* mem, registers_t* regs, u32 paddr, u32 val);
void ai_step(mem_t* mem, registers_t* regs, int cycles);