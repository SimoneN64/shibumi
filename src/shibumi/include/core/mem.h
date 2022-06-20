#pragma once
#include <common.h>
#include <mmio.h>
#include <stdlib.h>

typedef struct mem_t {
  u8 *cart, *rdram, *sram;
  u8 pif_ram[PIF_RAM_SIZE];
  u8 pif_bootrom[PIF_BOOTROM_SIZE];
  u8 isviewer[ISVIEWER_SIZE];
  size_t rom_mask;
  mmio_t mmio;
} mem_t;

void init_mem(mem_t* mem);
void destroy_mem(mem_t* mem);
bool load_rom(mem_t* mem, const char* path);
u8 read8_(mem_t* mem, registers_t* regs, u32 vaddr, s64 pc, bool tlb);
u16 read16_(mem_t* mem, registers_t* regs, u32 vaddr, s64 pc, bool tlb);
u32 read32_(mem_t* mem, registers_t* regs, u32 vaddr, s64 pc, bool tlb);
u64 read64_(mem_t* mem, registers_t* regs, u32 vaddr, s64 pc, bool tlb);
void write8_(mem_t* mem, registers_t* regs, u32 vaddr, u8 val, s64 pc, bool tlb);
void write16_(mem_t* mem, registers_t* regs, u32 vaddr, u16 val, s64 pc, bool tlb);
void write32_(mem_t* mem, registers_t* regs, u32 vaddr, u32 val, s64 pc, bool tlb);
void write64_(mem_t* mem, registers_t* regs, u32 vaddr, u64 val, s64 pc, bool tlb);

#define read8(mem, regs, vaddr, pc) read8_(mem, regs, vaddr, pc, true)
#define read8_physical(mem, regs, paddr) read8_(mem, regs, paddr, 0, false)
#define read16(mem, regs, vaddr, pc) read16_(mem, regs, vaddr, pc, true)
#define read16_physical(mem, regs, paddr) read16_(mem, regs, paddr, 0, false)
#define read32(mem, regs, vaddr, pc) read32_(mem, regs, vaddr, pc, true)
#define read32_physical(mem, regs, paddr) read32_(mem, regs, paddr, 0, false)
#define read64(mem, regs, vaddr, pc) read64_(mem, regs, vaddr, pc, true)
#define read64_physical(mem, regs, paddr) read64_(mem, regs, paddr, 0, false)

#define write8(mem, regs, vaddr, val, pc) write8_(mem, regs, vaddr, val, pc, true)
#define write8_physical(mem, regs, vaddr, val) write8_(mem, regs, vaddr, val, 0, false)
#define write16(mem, regs, vaddr, val, pc) write16_(mem, regs, vaddr, val, pc, true)
#define write16_physical(mem, regs, vaddr, val) write16_(mem, regs, vaddr, val, 0, false)
#define write32(mem, regs, vaddr, val, pc) write32_(mem, regs, vaddr, val, pc, true)
#define write32_physical(mem, regs, vaddr, val) write32_(mem, regs, vaddr, val, 0, false)
#define write64(mem, regs, vaddr, val, pc) write64_(mem, regs, vaddr, val, pc, true)
#define write64_physical(mem, regs, vaddr, val) write64_(mem, regs, vaddr, val, 0, false)
