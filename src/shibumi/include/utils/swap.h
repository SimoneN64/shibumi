#pragma once
#include <common.h>
#include <stdlib.h>
#include <log.h>
#define Z64 0x80
#define N64 0x40
#define V64 0x37
#define bswap_16(x) (((x) << 8) | ((x) >> 8))
#define bswap_32(x) ((((x) << 24) | ((x) >> 24)) | (((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8)))

INLINE void swap(u8 endianness, size_t size, u8* rom) {
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