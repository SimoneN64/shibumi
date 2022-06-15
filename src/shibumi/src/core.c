#include <core.h>
#include <utils.h>

void init_core(core_t* core) {
  init_cpu(&core->cpu);
  init_mem(&core->mem);
  core->running = false;
}

void destroy_core(core_t* core) {
  destroy_mem(&core->mem);
}

void run_frame(core_t* core) {
  mmio_t* mmio = &core->mem.mmio;
  for (mmio->vi.current = 0; mmio->vi.current < 262; mmio->vi.current++) {
    //int frame_cycles = 0;
    for(int i = 0; i < 6000; i++) {
      step(&core->cpu, &core->mem);
      //ai_step(&core->mem, &core->cpu.regs, 1);
      //frame_cycles++;
    }

    if((mmio->vi.current & 0x3FE) == mmio->vi.intr) {
      interrupt_raise(&mmio->mi, &core->cpu.regs, VI);
    }

    //int missed = CYCLES_PER_FRAME - frame_cycles;
    //ai_step(&core->mem, &core->cpu.regs, missed);
  }
}