#include <core.h>
#include <disasm.h>
#include <utils.h>

void init_core(core_t* core) {
  init_cpu(&core->cpu);
  init_mem(&core->mem);
  core->running = false;
  core->stepping = false;
}

void run_frame(core_t* core) {
  for(int i = 0; i < 100000 && core->running && !core->stepping && (u32)core->cpu.regs.pc != core->break_addr; i++) {
    step(&core->cpu, &core->mem);
  }
}