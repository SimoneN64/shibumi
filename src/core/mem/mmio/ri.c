#include <ri.h>
#include <log.h>

void init_ri(ri_t *ri) {
  ri->mode = 0xE;
  ri->config = 0x40;
  ri->select = 0x14;
  ri->refresh = 0x63634;
}

static const u8 shift_amount[4] = { 24, 16, 8, 0 };

u8 ri_read8(ri_t* ri, u32 paddr) {
  switch(paddr) {
    case 0x04700000 ... 0x04700003:
      return ri->mode >> shift_amount[paddr & 0xf];
    case 0x04700004 ... 0x04700007:
      return ri->config >> shift_amount[paddr & 0xf];
    case 0x0470000C ... 0x0470000F:
      return ri->select >> shift_amount[paddr & 0xf];
    case 0x04700010 ... 0x04700013:
      return ri->refresh >> shift_amount[paddr & 0xf];
    default: log_(WARNING, "Unhandled RI[%08X] read\n", paddr); return 0;
  }
}

u32 ri_read(ri_t* ri, u32 paddr) {
  switch(paddr) {
    case 0x04700000: return ri->mode;
    case 0x04700004: return ri->config;
    case 0x0470000C: return ri->select;
    case 0x04700010: return ri->refresh;
    default: log_(WARNING, "Unhandled RI[%08X] read\n", paddr); return 0;
  }
}

void ri_write(ri_t* ri, u32 paddr, u32 value) {
  switch(paddr) {
    case 0x04700000: ri->mode = value; break;
    case 0x04700004: ri->config = value; break;
    case 0x0470000C: ri->select = value; break;
    case 0x04700010: ri->refresh = value; break;
    default: log_(FATAL, "Unimplemented RI[%08X] register (%08X)\n", paddr, value);
  }
}