#pragma once
#include <common.h>

typedef union {
  u32 raw;
  struct {
    unsigned halt:1;
    unsigned broke:1;
    unsigned dma_busy:1;
    unsigned dma_full:1;
    unsigned io_full:1;
    unsigned single_step:1;
    unsigned interrupt_on_break:1;
    unsigned signal_0_set:1;
    unsigned signal_1_set:1;
    unsigned signal_2_set:1;
    unsigned signal_3_set:1;
    unsigned signal_4_set:1;
    unsigned signal_5_set:1;
    unsigned signal_6_set:1;
    unsigned signal_7_set:1;
    unsigned:17;
  };
} sp_status_t;

typedef union {
  u32 raw;
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
} sp_status_write_t;
