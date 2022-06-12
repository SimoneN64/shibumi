#include <registers.h>
#include <string.h>

void init_registers(registers_t* regs) {
  memset(regs->gpr, 0, 32*8);
  memset((s64*)&regs->cp0, 0, 32*8);
  regs->old_pc = (s64)0xFFFFFFFFA4000040;
  regs->pc = regs->old_pc;
  regs->next_pc = regs->pc + 4;
  regs->lo = 0;
  regs->hi = 0;
  regs->gpr[11] = (s64)0xFFFFFFFFA4000040;
  regs->gpr[20] = 0x0000000000000001;
  regs->gpr[22] = 0x000000000000003F;
  regs->gpr[29] = (s64)0xFFFFFFFFA4001FF0;
  regs->cp0.Random = 0x0000001F;
  regs->cp0.Status.raw = 0x70400004;
  regs->cp0.PRId = 0x00000B00;
  regs->cp0.Config = 0x0006E463;
}

void set_pc(registers_t* regs, s64 value) {
  regs->pc = value;
  regs->next_pc = value + 4;
}