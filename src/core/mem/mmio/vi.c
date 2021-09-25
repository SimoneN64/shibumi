#include <vi.h>
#include <log.h>

void init_vi(vi_t* vi) {
  vi->status = 0xF;
  vi->origin = 0;
  vi->width = 320;
  vi->current = 0;
  vi->vsync = 0;
  vi->hsync = 0;
}

u32 vi_read(vi_t* vi, u32 paddr) {
  switch(paddr) {
    case 0x0440000: return vi->status;
    case 0x0440004: return vi->origin;
    case 0x0440008: return vi->width;
    case 0x0440010: return vi->current;
    case 0x0440018: return vi->vsync;
    case 0x044001C: return vi->hsync;
    default: logfatal("Unimplemented VI read (%08X)\n", paddr);
  }
}

void vi_write(vi_t* vi, u32 paddr, u32 val) {
  switch(paddr) {
    case 0x0440000: vi->status = val; break;
    case 0x0440004: vi->origin = val; break;
    case 0x0440008: vi->width = val; break;
    case 0x0440010: vi->current = val; break;
    case 0x0440018: vi->vsync = val; break;
    case 0x044001C: vi->hsync = val; break;
    default: logfatal("Unimplemented VI write (%08X)\n", paddr);
  }
}