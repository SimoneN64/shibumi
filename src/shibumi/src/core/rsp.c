#include <rsp.h>
#include <log.h>
#include <intr.h>
#include <mem.h>
#include <string.h>

void init_rsp(rsp_t* rsp) {
  rsp->sp_status.raw = 0;
  rsp->old_pc = rsp->pc = rsp->next_pc = 0;
}

u32 sp_read(rsp_t* rsp, u32 addr) {
  switch (addr) {
    case 0x04040010: return rsp->sp_status.raw;
    case 0x04080000: return rsp->pc;
    default: logfatal("Unimplemented SP register read %08X\n", addr);
  }
}

#define CLEAR_SET(val, clear, set) do { \
  if((clear) && (set)) logfatal("Can't be both clear and set\n"); \
  if(clear) (val) = 0; \
  if(set) (val) = 1; \
} while(0)

void sp_write(rsp_t* rsp, mem_t* mem, registers_t* regs, u32 addr, u32 value) {
  mi_t* mi = &mem->mmio.mi;
  switch (addr) {
    case 0x04040010: {
      sp_status_write_t write = {.raw = value};
      CLEAR_SET(rsp->sp_status.halt, write.clear_halt, write.set_halt);
      CLEAR_SET(rsp->sp_status.broke, write.clear_broke, false);
      if(write.clear_intr) interrupt_lower(mi, regs, SP);
      if(write.set_intr) interrupt_raise(mi, regs, SP);
      CLEAR_SET(rsp->sp_status.single_step, write.clear_sstep, write.set_sstep);
      CLEAR_SET(rsp->sp_status.interrupt_on_break, write.clear_intr_on_break, write.set_intr_on_break);
      CLEAR_SET(rsp->sp_status.signal_0_set, write.clear_signal_0, write.set_signal_0);
      CLEAR_SET(rsp->sp_status.signal_1_set, write.clear_signal_1, write.set_signal_1);
      CLEAR_SET(rsp->sp_status.signal_2_set, write.clear_signal_2, write.set_signal_2);
      CLEAR_SET(rsp->sp_status.signal_3_set, write.clear_signal_3, write.set_signal_3);
      CLEAR_SET(rsp->sp_status.signal_4_set, write.clear_signal_4, write.set_signal_4);
      CLEAR_SET(rsp->sp_status.signal_5_set, write.clear_signal_5, write.set_signal_5);
      CLEAR_SET(rsp->sp_status.signal_6_set, write.clear_signal_6, write.set_signal_6);
      CLEAR_SET(rsp->sp_status.signal_7_set, write.clear_signal_7, write.set_signal_7);
    } break;
    case 0x04080000: rsp->pc = value & 0xFFF; break;
    default: logfatal("Unimplemented SP register write %08X, val: %08X\n\n", addr, value);
  }
}