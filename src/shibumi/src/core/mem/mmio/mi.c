#include <mi.h>
#include <log.h>
#include <intr.h>
#include <registers.h>
#include <immintrin.h>

void init_mi(mi_t* mi) {
  mi->mi_intr_mask.raw = 0;
  mi->mi_intr.raw = 0;
  mi->mi_mode = 0;
}

u32 mi_read(mi_t* mi, u32 paddr) {
  switch(paddr & 0xF) {
    case 0x0: return mi->mi_mode & 0x3FF;
    case 0x4: return MI_VERSION_REG;
    case 0x8: return mi->mi_intr.raw & 0x3F;
    case 0xC: return mi->mi_intr_mask.raw & 0x3F;
    default: logfatal("Unhandled MI[%08X] read\n", paddr);
  }
}

void mi_write(mi_t* mi, registers_t* regs, u32 paddr, u32 val) {
  switch(paddr & 0xF) {
    case 0x0:
      mi->mi_mode &= 0xFFFFFF80;
      mi->mi_mode |= val & 0x7F;
      if (val & (1 << 7)) {
        mi->mi_mode &= ~(1 << 7);
      }

      if (val & (1 << 8)) {
        mi->mi_mode |= 1 << 7;
      }

      if (val & (1 << 9)) {
        mi->mi_mode &= ~(1 << 8);
      }

      if (val & (1 << 10)) {
        mi->mi_mode |= 1 << 8;
      }

      if (val & (1 << 11)) {
        interrupt_lower(mi, regs, DP);
      }

      if (val & (1 << 12)) {
        mi->mi_mode &= ~(1 << 9);
      }

      if (val & (1 << 13)) {
        mi->mi_mode |= 1 << 9;
      }
      break;
    case 0x4: break;
    case 0xC:
      for (int bit = 0; bit < 6; bit++) {
        int clearbit = bit << 1;
        int setbit = (bit << 1) + 1;

        if (val & (1 << clearbit)) {
          mi->mi_intr_mask.raw &= ~(1 << bit);
        }
        
        if (val & (1 << setbit)) {
          mi->mi_intr_mask.raw |= 1 << bit;
        }
      }

      update_interrupt(mi, regs);
      break;
    default: logfatal("Unhandled MI[%08X] write (%08X)\n", val, paddr);
  }
}