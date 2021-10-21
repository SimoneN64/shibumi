#pragma once
#include <common.h>

u16 raccess_16(u8* data, u32 index);
u32 raccess_32(u8* data, u32 index);
u64 raccess_64(u8* data, u32 index);
void waccess_16(u8* data, u32 index, u16 val);
void waccess_32(u8* data, u32 index, u32 val);
void waccess_64(u8* data, u32 index, u64 val);

#define raccess(size, data, index) raccess_##size(data, index)
#define waccess(size, data, index, val) waccess_##size(data, index, val)