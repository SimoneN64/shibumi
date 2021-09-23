#pragma once
#include <common.h>
#include <stdlib.h>
#define Z64 0x80371240
#define N64 0x40123780
#define V64 0x37804012
#define bswap_16(x) (((x) << 8) | ((x) >> 8))
#define bswap_32(x) ((((x) << 24) | ((x) >> 24)) | (((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8)))

void swap(u32 endianness, size_t size, u8* rom);