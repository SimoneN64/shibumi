#pragma once
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u32 (*memory_read)(void* usr_data, u32 addr, bool tlb);
typedef void (*memory_write)(void* usr_data, u32 addr, void* val, bool tlb);

void igMemoryView(memory_read funcr, memory_write funcw, void* usr_data);