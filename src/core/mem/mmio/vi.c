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

static const u8 shift_amount[4] = { 24, 16, 8, 0 };

u8 vi_read8(vi_t* vi, u32 paddr) {
  switch(paddr) {
    case 0x04400000 ... 0x04400003:
      return (vi->status.raw) >> shift_amount[paddr & 0xf];
    case 0x04400004 ... 0x04400007:
      return (vi->origin) >> shift_amount[paddr & 0xf];
    case 0x04400008 ... 0x0440000B:
      return (vi->width) >> shift_amount[paddr & 0xf];
    case 0x04400010 ... 0x04400013:
      return (vi->current) >> shift_amount[paddr & 0xf];
    case 0x04400018 ... 0x0440001B:
      return (vi->vsync) >> shift_amount[paddr & 0xf];
    case 0x0440001C ... 0x0440001F:
      return (vi->hsync) >> shift_amount[paddr & 0xf];
    default: return 0;
  }
}

void vi_write8(vi_t* vi, u8 val, u32 paddr) {
  switch(paddr) {
    case 0x04400000 ... 0x04400003:
      vi->status.raw &= ~(0xff << shift_amount[paddr & 0xf]);
      vi->status.raw |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04400004 ... 0x04400007:
      vi->origin &= ~(0xff << shift_amount[paddr & 0xf]);
      vi->origin |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04400008 ... 0x0440000B:
      vi->width &= ~(0xff << shift_amount[paddr & 0xf]);
      vi->width |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04400010 ... 0x04400013:
      vi->current &= ~(0xff << shift_amount[paddr & 0xf]);
      vi->current |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x04400018 ... 0x0440001B:
      vi->vsync &= ~(0xff << shift_amount[paddr & 0xf]);
      vi->vsync |= (val << shift_amount[paddr & 0xf]);
      break;
    case 0x0440001C ... 0x0440001F:
      vi->hsync &= ~(0xff << shift_amount[paddr & 0xf]);
      vi->hsync |= (val << shift_amount[paddr & 0xf]);
      break;
    default: break;
  }
}

u32 vi_read(vi_t* vi, u32 paddr) {
  switch(paddr) {
    case 0x04400000: return vi->status.raw;
    case 0x04400004: return vi->origin;
    case 0x04400008: return vi->width;
    case 0x04400010: return vi->current;
    case 0x04400018: return vi->vsync;
    case 0x0440001C: return vi->hsync;
    default: log_(WARNING, "Unhandled VI[%08X] read\n", paddr); return 0;
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
    default: log_(WARNING, "Unimplemented VI[%08X] write (%08X)\n", paddr, val);
  }
}