#include <si.h>
#include <log.h>

u32 si_read(si_t* si, mi_t* mi, u32 addr) {
  switch(addr) {
    case 0x04800018: {
      u32 value = 0;
      value |= (si->dma_busy << 0); // DMA busy
      value |= (false << 1); // IO read busy
      value |= (false << 3); // DMA error
      value |= (mi->mi_intr.si << 12); // SI interrupt
      return value;
    }
    default: log_(FATAL, "Unhandled SI[%08X] read\n", addr);
  }

  return 0;
}

void si_write(si_t* si, mi_t* mi, registers_t* regs, u32 addr, u32 val) {
  switch(addr) {
    case 0x04800018:
      interrupt_lower(mi, regs, SI);
      break;
    default: log_(FATAL, "Unhandled SI[%08X] write (%08X)\n", addr, val);
  }
}