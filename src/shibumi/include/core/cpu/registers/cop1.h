#pragma once
#include <cop0.h>
#include <string.h>

typedef union {
  struct {
    unsigned rounding_mode:2;
    unsigned flag_inexact_operation:1;
    unsigned flag_underflow:1;
    unsigned flag_overflow:1;
    unsigned flag_division_by_zero:1;
    unsigned flag_invalid_operation:1;
    unsigned enable_inexact_operation:1;
    unsigned enable_underflow:1;
    unsigned enable_overflow:1;
    unsigned enable_division_by_zero:1;
    unsigned enable_invalid_operation:1;
    unsigned cause_inexact_operation:1;
    unsigned cause_underflow:1;
    unsigned cause_overflow:1;
    unsigned cause_division_by_zero:1;
    unsigned cause_invalid_operation:1;
    unsigned cause_unimplemented_operation:1;
    unsigned:5;
    unsigned compare:1;
    unsigned fs:1;
    unsigned:7;
  } PACKED;

  struct {
    unsigned:7;
    unsigned enable:5;
    unsigned cause:6;
    unsigned:14;
  } PACKED;

  u32 raw;
} fcr31_t;

ASSERTWORD(fcr31_t);

typedef union {
  struct {
    s32 lo:32;
    s32 hi:32;
  } PACKED;

  s64 raw;
} fgr_t;

ASSERTDWORD(fgr_t);

typedef struct {
  u32 fcr0;
  fcr31_t fcr31;
  fgr_t fgr[32];
} cop1_t;

INLINE u32 get_cop1_reg_word(cop1_t* cop1, cop0_t* cop0, u8 index) {
  if(cop0->Status.fr) {
    return cop1->fgr[index].lo;
  } else {
    if(index & 1) {
      return cop1->fgr[index & ~1].hi;
    } else {
      return cop1->fgr[index].lo;
    }
  }
}

INLINE void set_cop1_reg_word(cop1_t* cop1, cop0_t* cop0, u8 index, u32 value) {
  if(cop0->Status.fr) {
    cop1->fgr[index].lo = value;
  } else {
    if(index & 1) {
      cop1->fgr[index & ~1].hi = value;
    } else {
      cop1->fgr[index].lo = value;
    }
  }
}

INLINE u64 get_cop1_reg_dword(cop1_t* cop1, cop0_t* cop0, u8 index) {
  if(!cop0->Status.fr) {
    index &= ~1;
  }
  
  return cop1->fgr[index].raw;
}

INLINE void set_cop1_reg_dword(cop1_t* cop1, cop0_t* cop0, u8 index, u64 value) {
  if(!cop0->Status.fr) {
    index &= ~1;
  } 
  
  cop1->fgr[index].raw = value;
}

INLINE void set_cop1_register_double(cop1_t *cop1, cop0_t *cop0, u8 index, double value) {
  u64 raw;
  memcpy(&raw, &value, sizeof(double));
  set_cop1_reg_dword(cop1, cop0, index, raw);
}

INLINE double get_cop1_register_double(cop1_t *cop1, cop0_t *cop0, u8 index) {
  double doublevalue;
  u64 raw = get_cop1_reg_dword(cop1, cop0, index);
  memcpy(&doublevalue, &raw, sizeof(double));
  return doublevalue;
}

INLINE void set_cop1_register_float(cop1_t *cop1, cop0_t *cop0, u8 index, float value) {
  u32 raw;
  memcpy(&raw, &value, sizeof(float));
  set_cop1_reg_word(cop1, cop0, index, raw);
}

INLINE float get_cop1_register_float(cop1_t *cop1, cop0_t *cop0, u8 index) {
  u32 raw = get_cop1_reg_word(cop1, cop0, index);
  float floatvalue;
  memcpy(&floatvalue, &raw, sizeof(float));
  return floatvalue;
}