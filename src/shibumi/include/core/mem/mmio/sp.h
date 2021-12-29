#pragma once
#include <common.h>
typedef struct registers_t registers_t;

typedef union sp_status_write {
  struct {
    unsigned clear_halt:1;
    unsigned set_halt:1;
    unsigned clear_broke:1;
    unsigned clear_intr:1;
    unsigned set_intr:1;
    unsigned clear_sstep:1;
    unsigned set_sstep:1;
    unsigned clear_intr_on_break:1;
    unsigned set_intr_on_break:1;
    unsigned clear_signal_0:1;
    unsigned set_signal_0:1;
    unsigned clear_signal_1:1;
    unsigned set_signal_1:1;
    unsigned clear_signal_2:1;
    unsigned set_signal_2:1;
    unsigned clear_signal_3:1;
    unsigned set_signal_3:1;
    unsigned clear_signal_4:1;
    unsigned set_signal_4:1;
    unsigned clear_signal_5:1;
    unsigned set_signal_5:1;
    unsigned clear_signal_6:1;
    unsigned set_signal_6:1;
    unsigned clear_signal_7:1;
    unsigned set_signal_7:1;
    unsigned:7;
  };
  u32 raw;
} sp_status_t;

ASSERTWORD(sp_status_t);

void init_sp(sp_t* sp);
void sp_write8(sp_t* sp, u8 val, u32 paddr);
u8 sp_read8(sp_t* sp, u32 paddr);
u32 sp_read(sp_t* sp, u32 paddr);
void sp_write(sp_t* sp, registers_t* regs, u32 paddr, u32 val);