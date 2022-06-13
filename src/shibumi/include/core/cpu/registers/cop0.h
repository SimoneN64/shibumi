#pragma once
#include <common.h>

#define STATUS_MASK 0xFF57FFFF
typedef struct cpu_t cpu_t;
typedef struct mem_t mem_t;

typedef union {
  u32 raw;
  struct {
    unsigned:8;
    unsigned interrupt_pending:8;
    unsigned:16;
  } PACKED;
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
      } PACKED;
      u8 raw;
    } ip;
    unsigned:12;
    unsigned cop_error:2;
    unsigned:1;
    unsigned branch_delay:1;
  } PACKED;
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

void init_cop0(cop0_t* cop0);
u32 get_cop0_reg_word(cop0_t* cop0, u8 index);
void set_cop0_reg_word(cpu_t* cpu, mem_t* mem, u8 index, u32 value);
