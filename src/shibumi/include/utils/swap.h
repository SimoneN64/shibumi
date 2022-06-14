#pragma once
#include <common.h>
#include <stdlib.h>
#include <log.h>
#include <string.h>
#define Z64 0x80371240
#define N64 0x40123780
#define V64 0x37804012

INLINE void swap(size_t size, u8* rom) {
  u32 endianness;
  memcpy(&endianness, rom, 4);
  endianness = be32toh(endianness);
  switch(endianness) {
    case V64:
      for(int i = 0; i < size; i += 2) {
        u16 original = *(u16*)&rom[i];
        *(u16*)&rom[i] = bswap_16(original);
      }
      break;
    case N64:
      for(int i = 0; i < size; i += 4) {
        u32 original = *(u32*)&rom[i];
        *(u32*)&rom[i] = bswap_32(original);
      }
      break;
    case Z64:
      break;
    default:
      logfatal("Unrecognized rom format! Make sure this is a valid Nintendo 64 ROM dump!\n");
  }
}