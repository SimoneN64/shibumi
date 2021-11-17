#include <tlb.h>
#include <log.h>

u32 vtp(u32 addr) {
  switch(addr) {
    case 0x80000000 ... 0xBFFFFFFF:  return addr & 0x1FFFFFFF;
    default: //logfatal("Unimplemented TLB region %08X!\n", addr);
      alloc_strlencpy(last_message, "Unimplemented TLB region %08X!", addr);
      last_message_type = FATAL;
      return 0;
  }
}