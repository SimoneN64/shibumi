#pragma once
#include <common.h>
#include <memory.h>

INLINE u16 raccess_16(u8* data, u32 index) {
  u16 result = 0;
  memcpy(&result, &data[index], sizeof(u16));
  return result;
}

INLINE u32 raccess_32(u8* data, u32 index) {
  u32 result = 0;
  memcpy(&result, &data[index], sizeof(u32));
  return result;
}

INLINE u64 raccess_64(u8* data, u32 index) {
  u64 hi, lo, result;
  memcpy(&hi, &data[index], sizeof(u32));
  memcpy(&lo, &data[index + sizeof(u32)], sizeof(u32));
  
  result = (hi << 32) | lo;
  return result;
}

INLINE void waccess_16(u8* data, u32 index, u16 val) {
  memcpy(&data[index], &val, sizeof(u16));
}

INLINE void waccess_32(u8* data, u32 index, u32 val) {
  memcpy(&data[index], &val, sizeof(u32));
}

INLINE void waccess_64(u8* data, u32 index, u64 val) {
  u32 lo = val & 0xFFFFFFFF;
  u32 hi = (val >> 32) & 0xFFFFFFFF;

  memcpy(&data[index], &hi, sizeof(u32));
  memcpy(&data[index + sizeof(u32)], &lo, sizeof(u32));
}

#define raccess(size, data, index) raccess_##size(data, index)
#define waccess(size, data, index, val) waccess_##size(data, index, val)