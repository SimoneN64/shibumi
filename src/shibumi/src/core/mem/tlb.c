#include <tlb.h>
#include <log.h>

u32 vtp(u32 addr) {
  if(addr >= 0x80000000 && addr <= 0xBFFFFFFF) {
    return addr & 0x1FFFFFFF;
  }
  logfatal("Unimplemented TLB region %08X!", addr);
}