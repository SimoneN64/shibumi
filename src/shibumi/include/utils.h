#pragma once
#include <utils/log.h>
#include <utils/swap.h>
#include <time.h>

INLINE u8 crc(const u8* data) {
  u8 crc = 0;

  for(int i = 0; i <= 32; i++) {
    for (int j = 7; j >= 0; j--) {
      u8 xor_val = ((crc & 0x80) != 0) ? 0x85 : 0;
      crc <<= 1;
      if(i < 32) {
        if((data[i] & (1 << j)) != 0) {
          crc |= 1;
        }
      }

      crc ^= xor_val;
    }
  }

  return crc;
}