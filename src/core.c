#include <core.h>

void init_core(core_t* core, const char* path) {
  init_cpu(&core->cpu);
  init_mem(&core->mem);
  load_rom(path, &core->mem);
}

void run_core(core_t* core) {
  for(int i = 0; i < 100000; i++) {
    step(&core->cpu, &core->mem);
  }
}