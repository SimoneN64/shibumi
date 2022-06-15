#include <si.h>
#include <log.h>
#include <string.h>
#include <mem.h>
#include <pif.h>

void init_si(si_t* si) {
  si->status.raw = 0;
  si->dram_addr = 0;
  si->controller.raw = 0;
}

u32 si_read(si_t* si, mi_t* mi, u32 addr) {
  switch(addr) {
    case 0x04800000: return si->dram_addr;
    case 0x04800018: {
      u32 val = 0;
      val |= si->status.dma_busy;
      val |= (0 << 1);
      val |= (0 << 3);
      val |= (si->status.intr << 12);
      return val;
    }
    default: return 0;
  }
}

void si_write(si_t* si, mem_t* mem, registers_t* regs, u32 addr, u32 val) {
  switch(addr) {
    case 0x04800000:
      si->dram_addr = val;
      break;
    case 0x04800004: {
      // if(!(si->status.raw & 3)) {
        process_pif_commands(mem);

        u8 pif_addr = (val & 0x7FC) & PIF_RAM_DSIZE;
        memcpy(&mem->rdram[si->dram_addr & RDRAM_DSIZE],
               &mem->pif_ram[pif_addr], 64);
        interrupt_raise(&mem->mmio.mi, regs, SI);
        si->status.intr = 1;
      } break;
    case 0x04800010: {
      //if(!(si->status.raw & 3)) {
        u8 pif_addr = (val & 0x7FC) & PIF_RAM_DSIZE;
        memcpy(&mem->pif_ram[pif_addr],
               &mem->rdram[si->dram_addr & RDRAM_DSIZE], 64);

        process_pif_commands(mem);
        interrupt_raise(&mem->mmio.mi, regs, SI);
        si->status.intr = 1;
      } break;
    case 0x04800018:
      interrupt_lower(&mem->mmio.mi, regs, SI);
      si->status.intr = 0;
      break;
    default: logfatal("Unhandled SI[%08X] write (%08X)\n", addr, val);
  }
}