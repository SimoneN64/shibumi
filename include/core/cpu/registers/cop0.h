#pragma once
#include <common.h>

typedef union {
  struct {
    unsigned:2;
    unsigned exc_code:5;
    unsigned:1;
    union {
      struct {
        unsigned ip0;
        unsigned ip1;
        unsigned ip2;
        unsigned ip3;
        unsigned ip4;
        unsigned ip5;
        unsigned ip6;
        unsigned ip7;
      };
      u8 raw;
    } ip;
    unsigned:12;
    unsigned cop_error:2;
    unsigned:1;
    unsigned branch_delay:1;
  };

  u32 raw;
} cause_t;

typedef union {
  struct {
    unsigned ie:1;
    unsigned exl:1;
    unsigned erl:1;
    unsigned ksu:2;
    unsigned ux:1;
    unsigned sx:1;
    unsigned kx:1;
    unsigned im:8;
    unsigned ds:9;
    unsigned re:1;
    unsigned fr:1;
    unsigned rp:1;
    unsigned cu0:1;
    unsigned cu1:1;
    unsigned cu2:1;
    unsigned cu3:1;
  };

  u32 raw;
} status_t;

typedef struct {
  s64 Index, Random, EntryLo0, EntryLo1, Context;
  s64 PageMask, Wired, r7, BadVaddr, Count, EntryHi;
  s64 Compare;
  status_t Status;
  cause_t Cause;
  s64 EPC, PRId, Config, LLAddr, WatchLo, WatchHi, XContext;
  s64 r21, r22, r23, r24, r25, ParityError, CacheError, TagLo, TagHi;
  s64 ErrorEPC, r31;
} cop0_t;