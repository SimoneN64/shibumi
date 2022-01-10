#include <mi.h>
#include <log.h>
#include <assert.h>
#include <intr.h>
#include <registers.h>

void init_mi(mi_t* mi) {
  mi->mi_intr_mask.raw = 0;
  mi->mi_intr.raw = 0;
  mi->mi_mode = 0;
}

static const u8 shift_amount[4] = { 24, 16, 8, 0 };

u8 mi_read8(mi_t *mi, u32 paddr) {
  switch(paddr) {
    case 0x04300000 ... 0x04300003:
      return (mi->mi_mode & 0x3FF) >> shift_amount[paddr & 0xf];
    case 0x04300004 ... 0x04300007:
      return (MI_VERSION_REG) >> shift_amount[paddr & 0xf];
    case 0x04300008 ... 0x0430000B:
      return (mi->mi_intr.raw & 0x3F) >> shift_amount[paddr & 0xf];
    case 0x0430000C ... 0x0430000F:
      return (mi->mi_intr_mask.raw & 0x3F) >> shift_amount[paddr & 0xf];
    default: return 0;
  }
}

void mi_write8(mi_t* mi, u8 val, u32 paddr) {
  switch(paddr) {
    case 0x04300000 ... 0x04300003:
      mi->mi_mode &= ~(0xff << shift_amount[paddr & 0xf]);
      mi->mi_mode |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x0430000C ... 0x0430000F:
      mi->mi_intr_mask.raw &= ~(0xff << shift_amount[paddr & 0xf]);
      mi->mi_intr_mask.raw |= (val << shift_amount[paddr & 0xf]);
      break;
    default: break;
  }
}

u32 mi_read(mi_t* mi, u32 paddr) {
  switch(paddr) {
    case 0x04300000: return mi->mi_mode & 0x3FF;
    case 0x04300004: return MI_VERSION_REG;
    case 0x04300008: return mi->mi_intr.raw & 0x3F;
    case 0x0430000C: return mi->mi_intr_mask.raw & 0x3F;
    default: log_(WARNING, "Unhandled MI[%08X] read\n", paddr); return 0;
  }
}

void mi_write(mi_t* mi, registers_t* regs, u32 paddr, u32 val) {
  switch(paddr) {
    case 0x04300000:
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
        mi->mi_intr.dp = false;
      }

      if (val & (1 << 12)) {
        mi->mi_mode &= ~(1 << 9);
      }

      if (val & (1 << 13)) {
        mi->mi_mode |= 1 << 9;
      }
      break;
    case 0x04300004: break;
    case 0x0430000C:
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

      if(mi->mi_intr_mask.raw != 0) {
        process_interrupt(mi, regs);
      }
      break;
    default: log_(FATAL, "Unhandled MI[%08X] write (%08X)\n", val, paddr);
  }
}