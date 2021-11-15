#pragma once
#include <utils/log.h>
#include <utils/swap.h>
#include <time.h>

#define RD(x) (((x) >> 11) & 0x1F)
#define RT(x) (((x) >> 16) & 0x1F)
#define RS(x) (((x) >> 21) & 0x1F)
#define clock_to_ms(ticks) (ticks/(double)CLOCKS_PER_SEC)*1000.0
#define ascii(byte) ((byte) >= 0x20 && (byte) < 0x7F) ? (byte) : '.'