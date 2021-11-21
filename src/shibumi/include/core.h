#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <core/cpu.h>
#include <core/mem.h>
#include <disasm.h>

typedef struct {
  cpu_t cpu;
  mem_t mem;
  u32 break_addr;
	bool running, stepping;
} core_t;

void init_core(core_t* core);
void run_frame(core_t* core);

#ifdef __cplusplus
}
#endif