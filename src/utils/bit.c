#include <bit.h>
#include <string.h>

size_t next_pow_2(size_t num) {
  // Taken from "Bit Twiddling Hacks" by Sean Anderson:
  // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
  --num;
  num |= num >> 1;
  num |= num >> 2;
  num |= num >> 4;
  num |= num >> 8;
  num |= num >> 16;
  return num + 1;
}

u16 raccess_16(u8* data, u32 index) {
  u16 result = 0;
  memcpy(&result, &data[index], 2);
  return result;
}

u32 raccess_32(u8* data, u32 index) {
  u32 result = 0;
  memcpy(&result, &data[index], 4);
  return result;
}

u64 raccess_64(u8* data, u32 index) {
  u64 result = 0;
  memcpy(&result, &data[index], 8);
  return result;
}

void waccess_16(u8* data, u32 index, u16 val) {
  memcpy(&data[index], &val, 2);
}

void waccess_32(u8* data, u32 index, u32 val) {
  memcpy(&data[index], &val, 4);
}

void waccess_64(u8* data, u32 index, u64 val) {
  memcpy(&data[index], &val, 8);
}