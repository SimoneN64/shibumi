#include <cpu.h>
#include <instructions.h>

void init_cpu(cpu_t *cpu) {
  init_registers(&cpu->regs);
}

void step(cpu_t *cpu, mem_t *mem) {
  cpu->regs.gpr[0] = 0;
  u32 instruction = read32(mem, cpu->regs.pc);
  cpu->regs.old_pc = cpu->regs.pc;
  cpu->regs.pc = cpu->regs.next_pc;
  cpu->regs.next_pc += 4;
  exec(cpu, mem);
}

void exec(cpu_t* cpu, mem_t* mem) {

}