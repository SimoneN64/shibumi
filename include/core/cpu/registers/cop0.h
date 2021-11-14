#pragma once
#include <common.h>

typedef union {
  struct {
    unsigned:8;
    unsigned interrupt_pending:8;
    unsigned:16;
  };
  struct {
    unsigned:2;
    unsigned exc_code:5;
    unsigned:1;
    union {
      struct{
        unsigned ip0:1;
        unsigned ip1:1;
        unsigned ip2:1;
        unsigned ip3:1;
        unsigned ip4:1;
        unsigned ip5:1;
        unsigned ip6:1;
        unsigned ip7:1;
      } PACKED;
      u8 raw;
    } ip;
    unsigned:12;
    unsigned cop_error:2;
    unsigned:1;
    unsigned branch_delay:1;
  };

  u32 raw;
} cop0_cause_t;

ASSERTWORD(cop0_cause_t);

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
  } PACKED;
  struct {
    unsigned:16;
    unsigned de:1;
    unsigned ce:1;
    unsigned ch:1;
    unsigned:1;
    unsigned sr:1;
    unsigned ts:1;
    unsigned bev:1;
    unsigned:1;
    unsigned its:1;
    unsigned:7;
  } PACKED;
  u32 raw;
} PACKED cop0_status_t;

ASSERTWORD(cop0_status_t);

typedef struct {
  u32 Index, Random, EntryLo0, EntryLo1, Context;
  u32 PageMask, Wired, r7;
  u64 BadVaddr, Count, EntryHi;
  u32 Compare;
  cop0_status_t Status;
  cop0_cause_t Cause;
  u64 EPC;
  u32 PRId, Config, LLAddr, WatchLo, WatchHi;
  u64 XContext;
  u32 r21, r22, r23, r24, r25, ParityError, CacheError, TagLo, TagHi;
  u64 ErrorEPC;
  u32 r31;
} cop0_t;

INLINE u32 get_cop0_reg_word(cop0_t* cop0, u8 index) {
  switch(index) {
    case 0: return cop0->Index;
    case 1: return cop0->Random;
    case 2: return cop0->EntryLo0;
    case 3: return cop0->EntryLo1;
    case 4: return cop0->Context;
    case 5: return cop0->PageMask;
    case 6: return cop0->Wired;
    case 7: return cop0->r7;
    case 8: return cop0->BadVaddr;
    case 9: return cop0->Count;
    case 10: return cop0->EntryHi;
    case 11: return cop0->Compare;
    case 12: return cop0->Status.raw;
    case 13: return cop0->Cause.raw;
    case 14: return cop0->EPC;
    case 15: return cop0->PRId;
    case 16: return cop0->Config;
    case 17: return cop0->LLAddr;
    case 18: return cop0->WatchLo;
    case 19: return cop0->WatchHi;
    case 20: return cop0->XContext;
    case 21: return cop0->r21;
    case 22: return cop0->r22;
    case 23: return cop0->r23;
    case 24: return cop0->r24;
    case 25: return cop0->r25;
    case 26: return cop0->ParityError;
    case 27: return cop0->CacheError;
    case 28: return cop0->TagLo;
    case 29: return cop0->TagHi;
    case 30: return cop0->ErrorEPC;
    case 31: return cop0->r31;
  }

  //silence warning
  return 0;
}

INLINE void set_cop0_reg_word(cop0_t* cop0, u8 index, u32 value) {
  switch(index) {
    case 0: cop0->Index = value; break;
    case 1: cop0->Random = value; break;
    case 2: cop0->EntryLo0 = value; break;
    case 3: cop0->EntryLo1 = value; break;
    case 4: cop0->Context = value; break;
    case 5: cop0->PageMask = value; break;
    case 6: cop0->Wired = value; break;
    case 7: cop0->r7 = value; break;
    case 8: cop0->BadVaddr = value; break;
    case 9: cop0->Count = value; break;
    case 10: cop0->EntryHi = value; break;
    case 11: cop0->Compare = value; break;
    case 12: cop0->Status.raw = value; break;
    case 13: cop0->Cause.raw = value; break;
    case 14: cop0->EPC = value; break;
    case 15: cop0->PRId = value; break;
    case 16: cop0->Config = value; break;
    case 17: cop0->LLAddr = value; break;
    case 18: cop0->WatchLo = value; break;
    case 19: cop0->WatchHi = value; break;
    case 20: cop0->XContext = value; break;
    case 21: cop0->r21 = value; break;
    case 22: cop0->r22 = value; break;
    case 23: cop0->r23 = value; break;
    case 24: cop0->r24 = value; break;
    case 25: cop0->r25 = value; break;
    case 26: cop0->ParityError = value; break;
    case 27: cop0->CacheError = value; break;
    case 28: cop0->TagLo = value; break;
    case 29: cop0->TagHi = value; break;
    case 30: cop0->ErrorEPC = value; break;
    case 31: cop0->r31 = value; break;
  }
}