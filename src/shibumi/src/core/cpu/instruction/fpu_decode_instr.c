#include <fpu_decode_instr.h>
#include <log.h>

void fpu_decode(registers_t* regs, u32 instr) {
  u8 mask_cop = (instr >> 21) & 0x1F;
  u8 mask_cop2 = instr & 0x3F;
  switch(mask_cop) {
    // 000r_rccc
    case 0x00: mfc1(regs, instr); break;
    case 0x02: cfc1(regs, instr); break;
    case 0x04: mtc1(regs, instr); break;
    case 0x06: ctc1(regs, instr); break;
    case 0x10:
      switch(mask_cop2) {
        case 0x21: cvtds(regs, instr); break;
        case 0x24: cvtws(regs, instr); break;
        case 0x25: cvtls(regs, instr); break;
        default: logfatal("Unimplemented COP1 CO instruction S[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    case 0x11:
      switch(mask_cop2) {
        case 0x20: cvtsd(regs, instr); break;
        case 0x24: cvtwd(regs, instr); break;
        case 0x25: cvtld(regs, instr); break;
        default: logfatal("Unimplemented COP1 CO instruction D[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    case 0x14:
      switch(mask_cop2) {
        case 0x20: cvtsw(regs, instr); break;
        case 0x21: cvtdw(regs, instr); break;
        default: logfatal("Unimplemented COP1 CO instruction W[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    case 0x15:
      switch(mask_cop2) {
        case 0x20: cvtsl(regs, instr); break;
        case 0x21: cvtdl(regs, instr); break;
        default: logfatal("Unimplemented COP1 CO instruction L[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    default: logfatal("Unimplemented COP1 instruction %d %d", mask_cop >> 4, mask_cop & 7);
  }
}