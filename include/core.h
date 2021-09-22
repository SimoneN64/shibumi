#pragma once
#include <core/cpu.h>
#include <core/mem.h>

typedef struct {
  cpu_t cpu;
  mem_t mem;
} core_t;

void init_core(core_t* core, const char* path);
void run_core(core_t* core);