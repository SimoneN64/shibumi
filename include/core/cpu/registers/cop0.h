#pragma once
#include <common.h>

typedef union {
  struct {
    unsigned:2;
    unsigned exc_code:5;
    unsigned:1;
    union {
      struct {
        unsigned ip0:1;
        unsigned ip1:1;
        unsigned ip2:1;
        unsigned ip3:1;
        unsigned ip4:1;
        unsigned ip5:1;
        unsigned ip6:1;
        unsigned ip7:1;
      };
      u8 raw;
    } ip;
    unsigned:12;
    unsigned cop_error:2;
    unsigned:1;
    unsigned branch_delay:1;
  };

  u32 raw;
} cop0_cause_t;

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
} cop0_status_t;

typedef struct {
  s64 Index, Random, EntryLo0, EntryLo1, Context;
  s64 PageMask, Wired, r7, BadVaddr, Count, EntryHi;
  s64 Compare;
  cop0_status_t Status;
  cop0_cause_t Cause;
  s64 EPC, PRId, Config, LLAddr, WatchLo, WatchHi, XContext;
  s64 r21, r22, r23, r24, r25, ParityError, CacheError, TagLo, TagHi;
  s64 ErrorEPC, r31;
} cop0_t;