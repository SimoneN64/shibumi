#include <si.h>
#include <log.h>

void init_si(si_t* si) {
  si->status.raw = 0;
}

u32 si_read(si_t* si, mi_t* mi, u32 addr) {
  switch(addr) {
    case 0x04800018: return si->status.raw;
    default: return 0;
  }
}

void si_write(si_t* si, mi_t* mi, registers_t* regs, u32 addr, u32 val) {
  switch(addr) {
    case 0x04800018:
      interrupt_lower(mi, regs, SI);
      break;
    default: logfatal("Unhandled SI[%08X] write (%08X)\n", addr, val);
  }
}