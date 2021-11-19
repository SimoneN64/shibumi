#pragma once
#include <common.h>
#include <mmio.h>
#include <stdlib.h>
#define BYTE_ADDR(x) ((x) ^ 3)
#define HALF_ADDR(x) ((x) ^ 2)

typedef struct mem_t {
  u8 *cart, *rdram, *sram;
  u8 dmem[DMEM_SIZE], imem[IMEM_SIZE], pif_ram[PIF_RAM_SIZE];
  u8 pif_bootrom[PIF_BOOTROM_SIZE];
  size_t rom_mask;
  mmio_t mmio;
  u8 addr_sa;
} mem_t;

void init_mem(mem_t* mem);
bool load_rom(mem_t* mem, const char* path);
u8 read8(mem_t* mem, u32 vaddr);
u16 read16(mem_t* mem, u32 vaddr);
u32 read32_(mem_t* mem, u32 vaddr, bool tlb);
u64 read64(mem_t* mem, u32 vaddr);
void write8(mem_t* mem, u32 vaddr, u8 val);
void write16(mem_t* mem, u32 vaddr, u16 val);
void write32(mem_t* mem, registers_t* regs, u32 vaddr, u32 val);
void write64(mem_t* mem, u32 vaddr, u64 val);

#define read32(mem, vaddr) read32_(mem, vaddr, true)
#define read32_ignore_tlb(mem, vaddr) read32_(mem, vaddr, false)