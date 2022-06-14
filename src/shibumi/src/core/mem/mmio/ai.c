#include <ai.h>
#include <log.h>
#include <string.h>
#include <mem.h>
#include <registers.h>

#define max(x, y) ((x) > (y) ? (x) : (y))

void init_ai(ai_t* ai) {
  ai->dma_enable = 0;
  ai->dac_rate = 0;
  ai->bitrate = 0;
  ai->dma_count = 0;
  ai->dma_address_carry = 0;
  ai->cycles = 0;
  memset(ai->dma_address, 0, 2);
  memset(ai->dma_length, 0, 2);
  ai->dac.frequency = 44100;
  ai->dac.period = CPU_FREQ / ai->dac.frequency;
  ai->dac.precision = 16;
}

u32 ai_read(ai_t* ai, u32 paddr) {
  switch(paddr) {
    case 0x04500004:
      return ai->dma_length[0];
    case 0x0450000C: { // STATUS
      u32 val = 0;
      val |= (ai->dma_count > 1);
      val |= 1 << 20;
      val |= 1 << 24;
      val |= (ai->dma_count > 0) << 30;
      val |= 1 << 31; //(ai->dma_count > 1) << 31;
      return val;
    }
    default: logfatal("Unhandled AI read at addr %08X\n", paddr);
  }
}

void ai_write(mem_t* mem, registers_t* regs, u32 paddr, u32 val) {
  ai_t* ai = &mem->mmio->ai;
  switch(paddr) {
    case 0x04500000:
      if(ai->dma_count < 2) {
        ai->dma_address[ai->dma_count] = val & 0xFFFFFF & ~7;
      }
      break;
    case 0x04500004: {
      u32 len = val & 0x3FFFF & ~7;
      if(ai->dma_count < 2 && len) {
        ai->dma_length[ai->dma_count] = len;
        ai->dma_count++;
      }
    } break;
    case 0x04500008:
      ai->dma_enable = val & 1;
      break;
    case 0x0450000C:
      interrupt_lower(&mem->mmio->mi, regs, AI);
      break;
    case 0x04500010: {
      u32 old_dac_freq = ai->dac.frequency;
      ai->dac_rate = val & 0x3FFF;
      ai->dac.frequency = max(1, CPU_FREQ / 2 / (ai->dac_rate + 1)) * 1.037;
      ai->dac.period = CPU_FREQ / ai->dac.frequency;
      if(old_dac_freq != ai->dac.frequency) {
        logdebug("New audio sample rate!\n");
      }
    } break;
    case 0x04500014:
      ai->bitrate = val & 0xF;
      break;
    default: logfatal("Unhandled AI write at addr %08X with val %08X\n", paddr, val);
  }
}

void ai_step(mem_t* mem, registers_t* regs, int cycles) {
  ai_t* ai = &mem->mmio->ai;
  ai->cycles += cycles;
  while(ai->cycles > ai->dac.period) {
    u32 address_lo = (ai->dma_address[0] + 4) & 0x1fff;
    ai->dma_address[0] = (ai->dma_address[0] & ~0x1fff) | address_lo;
    ai->dma_address_carry = (address_lo == 0);
    ai->dma_length[0] -= 4;

    if(!ai->dma_length[0]) {
      interrupt_raise(&mem->mmio->mi, regs, AI);
      if(--ai->dma_count > 0) { // If we have another DMA pending, start on that one.
        ai->dma_address[0] = ai->dma_address[1];
        ai->dma_length[0]  = ai->dma_length[1];
      }
    }

    ai->cycles -= ai->dac.period;
  }
}