#pragma once
#include <common.h>
#include <memory.h>

INLINE u16 raccess_16(u8* data, u32 index) {
  u16 result = 0;
  memcpy(&result, &data[index], sizeof(u16));
  return be16toh(result);
}

INLINE u32 raccess_32(u8* data, u32 index) {
  u32 result = 0;
  memcpy(&result, &data[index], sizeof(u32));
  return be32toh(result);
}

INLINE u64 raccess_64(u8* data, u32 index) {
  u64 result;
  memcpy(&result, &data[index], sizeof(u64));
  return be64toh(result);
}

INLINE void waccess_16(u8* data, u32 index, u16 val) {
  u64 temp = htobe16(val);
  memcpy(&data[index], &temp, sizeof(u16));
}

INLINE void waccess_32(u8* data, u32 index, u32 val) {
  u32 temp = htobe32(val);
  memcpy(&data[index], &temp, sizeof(u32));
}

INLINE void waccess_64(u8* data, u32 index, u64 val) {
  u64 temp = htobe64(val);
  memcpy(&data[index], &temp, sizeof(u64));
}

#define raccess(size, data, index) raccess_##size(data, index)
#define waccess(size, data, index, val) waccess_##size(data, index, val)