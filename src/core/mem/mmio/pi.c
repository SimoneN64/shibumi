#include <pi.h>
#include <log.h>
#include <mem.h>
#include <string.h>

void init_pi(pi_t* pi) {
  pi->status = 0;
  pi->cart_addr = 0;
  pi->dram_addr = 0;
  pi->rd_len = 0;
  pi->wr_len = 0;
}

static const u8 shift_amount[4] = { 24, 16, 8, 0 };

u8 pi_read8(mi_t* mi, pi_t* pi, u32 paddr) {
  switch(paddr) {
    case 0x04600000 ... 0x04600003:
      return (pi->dram_addr) >> shift_amount[paddr & 0xf];
    case 0x04600004 ... 0x04600007:
      return (pi->cart_addr) >> shift_amount[paddr & 0xf];
    case 0x04600008 ... 0x0460000B:
      return (pi->rd_len) >> shift_amount[paddr & 0xf];
    case 0x0460000C ... 0x0460000F:
      return (pi->wr_len) >> shift_amount[paddr & 0xf];
    case 0x04600010 ... 0x04600013: {
      u32 value = 0;
      value |= (pi->status & 1); // Is PI DMA active?
      value |= (0 << 1); // Is PI IO busy?
      value |= (0 << 2); // PI IO error?
      value |= (mi->mi_intr.pi << 3); // PI interrupt?
      return (value) >> shift_amount[paddr & 0xf];
    }
    case 0x04600014 ... 0x04600030:
      return (pi->stub[(paddr & 0xff) - 5]) >> shift_amount[paddr & 0xf];
    default: return 0;
  }
}

void pi_write8(mi_t* mi, pi_t* pi, u8 val, u32 paddr) {
  switch(paddr) {
    case 0x04600000 ... 0x04600003:
      pi->dram_addr &= ~(0xff << shift_amount[paddr & 0xf]);
      pi->dram_addr |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04600004 ... 0x04600007:
      pi->cart_addr &= ~(0xff << shift_amount[paddr & 0xf]);
      pi->cart_addr |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04600008 ... 0x0460000B:
      pi->rd_len &= ~(0xff << shift_amount[paddr & 0xf]);
      pi->rd_len |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x0460000C ... 0x0460000F:
      pi->wr_len &= ~(0xff << shift_amount[paddr & 0xf]);
      pi->wr_len |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04600010 ... 0x04600013:
      break;
    case 0x04600014 ... 0x04600030:
      pi->stub[(paddr & 0xff) - 5] &= ~(0xff << shift_amount[paddr & 0xf]);
      pi->stub[(paddr & 0xff) - 5] |= (val << shift_amount[paddr & 0xf]);
      break;
    default: break;
  }
}

u32 pi_read(mi_t* mi, pi_t* pi, u32 paddr) {
  switch(paddr) {
    case 0x04600000: return pi->dram_addr;
    case 0x04600004: return pi->cart_addr;
    case 0x04600008: return pi->rd_len;
    case 0x0460000C: return pi->wr_len;
    case 0x04600010: {
      u32 value = 0;
      value |= (pi->status & 1); // Is PI DMA active?
      value |= (0 << 1); // Is PI IO busy?
      value |= (0 << 2); // PI IO error?
      value |= (mi->mi_intr.pi << 3); // PI interrupt?
      return value;
    }
    case 0x04600014: case 0x04600018: case 0x0460001C: case 0x04600020:
    case 0x04600024: case 0x04600028: case 0x0460002C: case 0x04600030:
      return pi->stub[(paddr & 0xff) - 5];
    default: log_(WARNING, "Unhandled PI[%08X] read\n", paddr); return 0;
  }
}

void pi_write(mem_t* mem, registers_t* regs, u32 paddr, u32 val) {
  pi_t* pi = &mem->mmio.pi;
  mi_t* mi = &mem->mmio.mi;
  memory_regions_t* memory_regions = &mem->memory_regions;
  switch(paddr) {
    case 0x04600000: pi->dram_addr = val; break;
    case 0x04600004: pi->cart_addr = val; break;
    case 0x04600008: {
      u32 len = (val & 0x00FFFFFF) + 1;
      u32 cart_addr = pi->cart_addr & 0xFFFFFFFE;
      u32 dram_addr = pi->dram_addr & 0x007FFFFE;
      if (dram_addr & 0x7) {
        len -= dram_addr & 0x7;
      }
      pi->rd_len = len;
      memcpy(&memory_regions->cart[cart_addr & mem->rom_mask], &memory_regions->rdram[dram_addr & RDRAM_DSIZE], len);
      pi->dram_addr = dram_addr + len;
      pi->cart_addr = cart_addr + len;
      interrupt_raise(mi, regs, PI);
      pi->status = pi->status & 0xFFFFFFFE;
      log_(INFO, "PI DMA from rdram to cart (size: %.2f MiB)\n", (float)len / 1048576);
    } break;
    case 0x0460000C: {
      u32 len = (val & 0x00FFFFFF) + 1;
      u32 cart_addr = pi->cart_addr & 0xFFFFFFFE;
      u32 dram_addr = pi->dram_addr & 0x007FFFFE;
      if (dram_addr & 0x7) {
        len -= dram_addr & 0x7;
      }
      pi->wr_len = len;
      memcpy(&memory_regions->rdram[dram_addr & RDRAM_DSIZE], &memory_regions->cart[cart_addr & mem->rom_mask], len);
      pi->dram_addr = dram_addr + len;
      pi->cart_addr = cart_addr + len;
      interrupt_raise(mi, regs, PI);
      pi->status = pi->status & 0xFFFFFFFE;
      log_(INFO, "PI DMA from cart to rdram (size: %.2f MiB)\n", (float)len / 1048576);
    } break;
    case 0x04600010:
    if(val & 2) {
      interrupt_lower(mi, regs, PI);
    } break;
    case 0x04600014: case 0x04600018: case 0x0460001C: case 0x04600020:
    case 0x04600024: case 0x04600028: case 0x0460002C: case 0x04600030:
      pi->stub[(paddr & 0xff) - 5] = val & 0xff;
      break;
    default: log_(FATAL, "Unhandled PI[%08X] write (%08X)\n", val, paddr);
  }
}