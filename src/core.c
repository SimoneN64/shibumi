#include <core.h>
#include <disasm.h>
#include <utils.h>

void init_core(core_t* core) {
  init_cpu(&core->cpu);
  init_mem(&core->mem);
  core->running = false;
  core->breakpoint = false;
  core->stepping = false;
}

void run_frame(core_t* core) {
  s64 iterations = core->breakpoint ? max(core->cpu.regs.pc, core->break_addr) - min(core->cpu.regs.pc, core->break_addr) : 100000;
  for(int i = 0; i < iterations && core->running && !core->stepping; i++) {
    step(&core->cpu, &core->mem);
  }
}