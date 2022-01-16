#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <core/cpu.h>
#include <core/mem.h>

typedef struct core_t {
  cpu_t cpu;
  mem_t mem;
	bool running, stepping;
} core_t;

void init_core(core_t* core);

#ifdef __cplusplus
}
#endif