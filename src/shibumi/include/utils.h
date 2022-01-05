#pragma once
#include <utils/log.h>
#include <utils/swap.h>
#include <time.h>

#define clock_to_ms(ticks) (ticks/(double)CLOCKS_PER_SEC)*1000.0
#define ascii(byte) ((byte) >= 0x20 && (byte) < 0x7F) ? (byte) : '.'