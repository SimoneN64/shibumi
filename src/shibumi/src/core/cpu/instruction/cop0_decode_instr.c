#include <cop0_decode_instr.h>
#include <log.h>

void cop0_decode(registers_t* regs, u32 instr) {
  u8 mask_cop = (instr >> 21) & 0x1F;
  u8 mask_cop2 = instr & 0x3F;
  switch(mask_cop) {
    case 0x00: mfc0(regs, instr); break;
    case 0x04: mtc0(regs, instr); break;
    case 0x10 ... 0x1F:
      switch(mask_cop2) {
        case 0x02: break;
        case 0x18: eret(regs); break;
        default: logfatal("Unimplemented COP0 CO instruction %d %d (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    default: logfatal("Unimplemented COP0 instruction %d %d", mask_cop >> 4, mask_cop & 7);
  }
}