#include <cpu.h>
#include <instruction/decode_instr.h>
#include <utils.h>

void init_cpu(cpu_t *cpu) {
  init_registers(&cpu->regs);
}

void step(cpu_t *cpu, mem_t *mem) {
  registers_t* regs = &cpu->regs;
  regs->gpr[0] = 0;
  u32 instruction = read32(mem, regs->pc, regs->pc);
  regs->old_pc = regs->pc;
  regs->pc = regs->next_pc;
  regs->next_pc += 4;
  exec(cpu, mem, instruction);
  handle_interrupt(cpu, mem);
}

INLINE bool should_service_interrupt(registers_t* regs) {
  bool interrupts_pending = (regs->cp0.Status.im & regs->cp0.Cause.ip.raw) != 0;
  bool interrupts_enabled = regs->cp0.Status.ie == 1;
  bool currently_handling_exception = regs->cp0.Status.exl == 1;
  bool currently_handling_error = regs->cp0.Status.erl == 1;

  return interrupts_pending && interrupts_enabled &&
         !currently_handling_exception && !currently_handling_error;
}

void handle_interrupt(cpu_t* cpu, mem_t* mem) {
  if(should_service_interrupt(&cpu->regs)) {
    logfatal("Should service interrupt!");
  }
}