#include <swap.h>
#include <utils.h>

void swap(u32 endianness, u8* rom) {
  switch(endianness) {
  case V64:
  for(int i = 0; i < sizeof(rom); i += 2) {
    u16 original = *(u16*)&rom[i];
    *(u16*)&rom[i] = bswap_16(original);
  }
  break;
  case N64:
  break;
  case Z64:
  for(int i = 0; i < sizeof(rom); i += 4) {
    u32 original = *(u32*)&rom[i];
    *(u32*)&rom[i] = bswap_32(original);
  }
  break;
  default:
    logfatal("Unrecognized rom format! Make sure this is a valid Nintendo 64 ROM dump!\n");
  }
}