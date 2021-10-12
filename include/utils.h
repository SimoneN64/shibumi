#pragma once
#include <utils/log.h>
#include <utils/swap.h>
#define RD(x) (((x) >> 11) & 0x1F)
#define RT(x) (((x) >> 16) & 0x1F)
#define RS(x) (((x) >> 21) & 0x1F)
#define sx(x, a) (((x) << (a)) >> (a))