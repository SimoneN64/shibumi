#pragma once
#include <common.h>

typedef union {
  struct {
    unsigned rm:2;
    union {
      struct {
        unsigned i:1;
        unsigned u:1;
        unsigned o:1;
        unsigned z:1;
        unsigned v:1;
      };
      unsigned raw:5;
    } flags, enables;
    union {
      struct {
        unsigned i:1;
        unsigned u:1;
        unsigned o:1;
        unsigned z:1;
        unsigned v:1;
        unsigned e:1;
      };
      unsigned raw:6;
    } cause;
    unsigned:5;
    unsigned c:1;
    unsigned fs:1;
    unsigned:7;
  };

  u32 raw;
} cop1_status_t;

typedef union {
  struct {
    unsigned rev:8;
    unsigned imp:8;
    unsigned:16;
  };
  u32 raw;
} cop1_revision_t;

typedef union {
  struct {
    u32 low, hi;
  };

  u64 reg;
} fgr_t;

typedef struct {
  cop1_revision_t fcr0;
  u32 fcr[30];
  cop1_status_t fcr31;
} fcr_t;

typedef struct {
  fgr_t fgr[32];
  fcr_t fcr;
} cop1_t;