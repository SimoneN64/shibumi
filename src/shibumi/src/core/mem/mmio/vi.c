#include <vi.h>
#include <log.h>
#include <intr.h>

void init_vi(vi_t* vi) {
  vi->status.raw = 0xF;
  vi->intr = 256;
  vi->origin = 0;
  vi->width = 320;
  vi->current = 0;
  vi->vsync = 0;
  vi->hsync = 0;
  vi->num_halflines = 262;
  vi->num_fields = 1;
  vi->cycles_per_halfline = 1000;
}

u32 vi_read(vi_t* vi, u32 paddr) {
  switch(paddr) {
    case 0x04400000: return vi->status.raw;
    case 0x04400004: return vi->origin;
    case 0x04400008: return vi->width;
    case 0x0440000C: return vi->intr;
    case 0x04400010: return vi->current << 1;
    case 0x04400018: return vi->vsync;
    case 0x0440001C: return vi->hsync;
    default:
      logdebug("Unsupported VI[%08X] read\n", paddr);
      return 0;
  }
}

void vi_write(mi_t* mi, registers_t* regs, vi_t* vi, u32 paddr, u32 val) {
  switch(paddr) {
    case 0x04400000:
      vi->status.raw = val;
      vi->num_fields = vi->status.serrate ? 2 : 1;
      break;
    case 0x04400004: {
      u32 masked = val & 0xFFFFFF;
      if(vi->origin != masked) {
        vi->swaps++;
      }
      vi->origin = masked;
    } break;
    case 0x04400008: {
      vi->width = val & 0x7FF;
    } break;
    case 0x0440000C: {
      vi->intr = val & 0x3FF;
    } break;
    case 0x04400010:
      interrupt_lower(mi, regs, VI);
      break;
    case 0x04400018: {
      vi->vsync = val & 0x3FF;
      vi->num_halflines = vi->vsync >> 1;
      vi->cycles_per_halfline = CYCLES_PER_FRAME / vi->num_halflines;
    } break;
    case 0x0440001C: {
      vi->hsync = val & 0x3FF;
    } break;
    default:
      logdebug("Unimplemented VI[%08X] write (%08X)\n", paddr, val);
  }
}