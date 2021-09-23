#pragma once
#include <core/cpu.h>
#include <core/mem.h>
#include <disasm.h>

typedef struct core_t {
  cpu_t cpu;
  mem_t mem;
  disasm_t disasm;
} core_t;

void init_core(core_t* core, const char* path);
void run_core(core_t* core);