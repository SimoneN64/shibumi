#include <fpu_decode_instr.h>
#include <log.h>
#include <instructions.h>
#include <cpu.h>

void fpu_decode(cpu_t* cpu, u32 instr) {
  registers_t* regs = &cpu->regs;
  u8 mask_cop = (instr >> 21) & 0x1F;
  u8 mask_cop2 = instr & 0x3F;
  u8 mask_branch = (instr >> 16) & 0x1F;
  switch(mask_cop) {
    // 000r_rccc
    case 0x00: mfc1(regs, instr); break;
    case 0x02: cfc1(regs, instr); break;
    case 0x04: mtc1(regs, instr); break;
    case 0x06: ctc1(regs, instr); break;
    case 0x08:
      switch(mask_branch) {
        case 0: b(cpu, instr, !regs->cp1.fcr31.compare); break;
        case 1: b(cpu, instr, regs->cp1.fcr31.compare); break;
        case 2: bl(cpu, instr, !regs->cp1.fcr31.compare); break;
        case 3: bl(cpu, instr, regs->cp1.fcr31.compare); break;
        default: logfatal("Undefined BC COP1 %02X\n", mask_branch);
      }
      break;
    case 0x10: // s
      switch(mask_cop2) {
        case 0x00: adds(regs, instr); break;
        case 0x01: subs(regs, instr); break;
        case 0x02: muls(regs, instr); break;
        case 0x03: divs(regs, instr); break;
        case 0x05: abss(regs, instr); break;
        case 0x06: movs(regs, instr); break;
        case 0x09: truncls(regs, instr); break;
        case 0x0A: ceills(regs, instr); break;
        case 0x0D: truncws(regs, instr); break;
        case 0x0E: ceilws(regs, instr); break;
        case 0x21: cvtds(regs, instr); break;
        case 0x24: cvtws(regs, instr); break;
        case 0x25: cvtls(regs, instr); break;
        case 0x32: cconds(regs, instr, EQ); break;
        case 0x39: cconds(regs, instr, NGLE); break;
        case 0x3B: cconds(regs, instr, NGL); break;
        case 0x3C: cconds(regs, instr, LT); break;
        case 0x3D: cconds(regs, instr, NGE); break;
        case 0x3E: cconds(regs, instr, LE); break;
        case 0x3F: cconds(regs, instr, NGT); break;
        default: logfatal("Unimplemented COP1 CO instruction S[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    case 0x11: // d
      switch(mask_cop2) {
        case 0x00: addd(regs, instr); break;
        case 0x01: subd(regs, instr); break;
        case 0x02: muld(regs, instr); break;
        case 0x03: divd(regs, instr); break;
        case 0x05: absd(regs, instr); break;
        case 0x06: movd(regs, instr); break;
        case 0x09: truncld(regs, instr); break;
        case 0x0A: ceilld(regs, instr); break;
        case 0x0D: truncwd(regs, instr); break;
        case 0x0E: ceilwd(regs, instr); break;
        case 0x20: cvtsd(regs, instr); break;
        case 0x24: cvtwd(regs, instr); break;
        case 0x25: cvtld(regs, instr); break;
        case 0x32: ccondd(regs, instr, EQ); break;
        case 0x39: ccondd(regs, instr, NGLE); break;
        case 0x3B: ccondd(regs, instr, NGL); break;
        case 0x3C: ccondd(regs, instr, LT); break;
        case 0x3D: ccondd(regs, instr, NGE); break;
        case 0x3E: ccondd(regs, instr, LE); break;
        case 0x3F: ccondd(regs, instr, NGT); break;
        default: logfatal("Unimplemented COP1 CO instruction D[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    case 0x14: // w
      switch(mask_cop2) {
        case 0x01: subw(regs, instr); break;
        case 0x05: absw(regs, instr); break;
        case 0x02: mulw(regs, instr); break;
        case 0x06: movw(regs, instr); break;
        case 0x20: cvtsw(regs, instr); break;
        case 0x21: cvtdw(regs, instr); break;
        default: logfatal("Unimplemented COP1 CO instruction W[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    case 0x15: // l
      switch(mask_cop2) {
        case 0x01: subl(regs, instr); break;
        case 0x05: absl(regs, instr); break;
        case 0x02: mull(regs, instr); break;
        case 0x06: movl(regs, instr); break;
        case 0x20: cvtsl(regs, instr); break;
        case 0x21: cvtdl(regs, instr); break;
        default: logfatal("Unimplemented COP1 CO instruction L[%d %d] (%08X) (%016lX)", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    default: logfatal("Unimplemented COP1 instruction %d %d", mask_cop >> 3, mask_cop & 7);
  }
}