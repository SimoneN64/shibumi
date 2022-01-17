#include <core.h>
#include <utils.h>

void init_core(core_t* core) {
  init_cpu(&core->cpu);
  init_mem(&core->mem);
  core->running = false;
}

void run_frame(core_t* core) {
  for (int i = 0; i < 100000; i++) {
    step(&core->cpu, &core->mem);
  }
}