#pragma once
#include <sp_status.h>
#include <mi.h>

typedef struct registers_t registers_t;

typedef struct {
  sp_status_t sp_status;
  u16 old_pc, pc, next_pc;
} rsp_t;

void init_rsp(rsp_t* rsp);
u32 sp_read(rsp_t* rsp, u32 addr);
void sp_write(rsp_t* rsp, mi_t* mi, registers_t* regs, u32 addr, u32 value);
