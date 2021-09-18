#pragma once
#include <common.h>
#include <stdlib.h>

typedef struct {
  u8 *cart, *rdram, *sram;
  u8 dmem[DMEM_SIZE], imem[IMEM_SIZE], pif_ram[PIF_RAM_SIZE];
  u8 pif_bootrom[PIF_BOOTROM_SIZE];
  size_t rom_size;
} mem_t;

void init_mem(mem_t* mem);
void load_rom(const char* path, mem_t* mem);
u8 read8(mem_t* mem);
u16 read16(mem_t* mem);
u32 read32(mem_t* mem);
void write8(mem_t* mem, u8 val);
void write16(mem_t* mem, u16 val);
void write32(mem_t* mem, u32 val);