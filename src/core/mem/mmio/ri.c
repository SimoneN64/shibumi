#include <ri.h>
#include <log.h>

void init_ri(ri_t *ri) {
  ri->mode = 0xE;
  ri->config = 0x40;
  ri->select = 0x14;
  ri->refresh = 0x63634;
}

u32 ri_read(ri_t* ri, u32 paddr) {
  switch(paddr) {
    case 0x04700000: return ri->mode;
    case 0x04700004: return ri->config;
    case 0x0470000C: return ri->select;
    case 0x04700010: return ri->refresh;
    default: logfatal("Unimplemented RI register (%08X)\n", paddr);
  }
}

void ri_write(ri_t* ri, u32 paddr, u32 value) {
  switch(paddr) {
    case 0x04700000: ri->mode = value; break;
    case 0x04700004: ri->config = value; break;
    case 0x0470000C: ri->select = value; break;
    case 0x04700010: ri->refresh = value; break;
    default: logfatal("Unimplemented RI register (%08X)\n", paddr);
  }
}