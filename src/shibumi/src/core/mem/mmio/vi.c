#include <vi.h>
#include <log.h>

void init_vi(vi_t* vi) {
  vi->status.raw = 0xF;
  vi->origin = 0;
  vi->width = 320;
  vi->current = 0;
  vi->vsync = 0;
  vi->hsync = 0;
}

u32 vi_read(vi_t* vi, u32 paddr) {
  switch(paddr) {
    case 0x04400000: return vi->status.raw;
    case 0x04400004: return vi->origin;
    case 0x04400008: return vi->width;
    case 0x04400010: return vi->current;
    case 0x04400018: return vi->vsync;
    case 0x0440001C: return vi->hsync;
    default:
      logdebug("Unhandled VI[%08X] read\n", paddr);
      return 0;
  }
}

void vi_write(vi_t* vi, u32 paddr, u32 val) {
  switch(paddr) {
    case 0x04400000: vi->status.raw = val; break;
    case 0x04400004: vi->origin = val; break;
    case 0x04400008: vi->width = val; break;
    case 0x04400010: vi->current = val; break;
    case 0x04400018: vi->vsync = val; break;
    case 0x0440001C: vi->hsync = val; break;
    default:
      logdebug("Unimplemented VI[%08X] write (%08X)\n", paddr, val);
  }
}