#include <core.h>
#include <disasm.h>
#include <utils/log.h>

void init_core(core_t* core, const char* path) {
  init_cpu(&core->cpu);
  init_mem(&core->mem);
  init_disasm(&core->disasm);
  load_rom(path, &core->mem);
}

void run_core(core_t* core) {
  for(int i = 0; i < 100000; i++) {
    step(&core->cpu, &core->mem);
    logdebug("%s", disasm_run(core, &core->disasm));
  }
}