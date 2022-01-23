#include <cop0_instructions.h>
#include <cpu.h>

void mtc0(cpu_t* cpu, mem_t* mem, u32 instr) {
  set_cop0_reg_word(cpu, mem, RD(instr), cpu->regs.gpr[RT(instr)]);
}

void mfc0(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = (s32)get_cop0_reg_word(&regs->cp0, RD(instr));
}

void eret(registers_t* regs) {
  if(regs->cp0.Status.erl) {
    set_pc(regs, (s64)regs->cp0.ErrorEPC);
    regs->cp0.Status.erl = false;
  } else {
    set_pc(regs, (s64)regs->cp0.EPC);
    regs->cp0.Status.exl = false;
  }
}