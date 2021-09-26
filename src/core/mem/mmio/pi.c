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

u32 pi_read(pi_t* pi, u32 paddr) {
  switch(paddr) {
    case 0x04600000: return pi->dram_addr;
    case 0x04600004: return pi->cart_addr;
    case 0x04600008: return pi->rd_len;
    case 0x0460000C: return pi->wr_len;
    case 0x04600010: return pi->status & 7;
    default: logfatal("Unhandled PI read (%08X)", paddr);
  }
}

void pi_write(mem_t* mem, registers_t* regs, u32 paddr, u32 val) {
  pi_t* pi = &mem->mmio.pi;
  mi_t* mi = &mem->mmio.mi;
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
      memcpy(&mem->cart[cart_addr & mem->rom_size], &mem->rdram[dram_addr & RDRAM_DSIZE], len);
      pi->dram_addr = dram_addr + len;
      pi->cart_addr = cart_addr + len;
      printf("PI DMA from rdram to cart (size: %.2f MiB)\n", (float)len / 1048576);
    } break;
    case 0x0460000C: {
      u32 len = (val & 0x00FFFFFF) + 1;
      u32 cart_addr = pi->cart_addr & 0xFFFFFFFE;
      u32 dram_addr = pi->dram_addr & 0x007FFFFE;
      if (dram_addr & 0x7) {
        len -= dram_addr & 0x7;
      }
      pi->wr_len = len;
      memcpy(&mem->rdram[dram_addr & RDRAM_DSIZE], &mem->cart[cart_addr & mem->rom_size], len);
      pi->dram_addr = dram_addr + len;
      pi->cart_addr = cart_addr + len;
      printf("PI DMA from cart to rdram (size: %.2f MiB)\n", (float)len / 1048576);
    } break;
    case 0x04600010:
    if(val & 2) {
      interrupt_lower(mi, regs, PI);
    }
    break;
    default: logfatal("Unhandled PI write (%08X)(%08X)", val, paddr);
  }
}