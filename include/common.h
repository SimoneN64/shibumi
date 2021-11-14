#pragma once
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef __uint128_t u128;
typedef __int128_t s128;

#define RDRAM_SIZE 0x800000
#define RDRAM_DSIZE (RDRAM_SIZE - 1)
#define SRAM_SIZE 0x8000000
#define SRAM_DSIZE (SRAM_SIZE - 1)
#define DMEM_SIZE 0x1000
#define DMEM_DSIZE (DMEM_SIZE - 1)
#define IMEM_SIZE 0x1000
#define IMEM_DSIZE (IMEM_SIZE - 1)
#define PIF_RAM_SIZE 0x40
#define PIF_RAM_DSIZE (PIF_RAM_SIZE - 1)
#define PIF_BOOTROM_SIZE 0x7C0
#define PIF_BOOTROM_DSIZE (PIF_BOOTROM_SIZE - 1)
#ifndef EMU_DIR
#define EMU_DIR ""
#endif