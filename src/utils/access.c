#include <access.h>
#include <string.h>

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