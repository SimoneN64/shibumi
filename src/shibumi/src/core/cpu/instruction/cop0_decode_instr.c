#include <cop0_decode_instr.h>
#include <log.h>
#include <cpu.h>

void cop0_decode(cpu_t* cpu, mem_t* mem, u32 instr) {
  u8 mask_cop = (instr >> 21) & 0x1F;
  u8 mask_cop2 = instr & 0x3F;
  registers_t* regs = &cpu->regs;
  switch(mask_cop) {
    case 0x00: mfc0(regs, instr); break;
    case 0x01: dmfc0(regs, instr); break;
    case 0x04: mtc0(regs, instr); break;
    case 0x05: dmtc0(regs, instr); break;
    case 0x10 ... 0x1F:
      switch(mask_cop2) {
        case 0x01: tlbr(regs); break;
        case 0x02: tlbwi(regs); break;
        case 0x08: tlbp(regs); break;
        case 0x18: eret(regs); break;
        default: logfatal("Unimplemented COP0 function %d %d (%08X) (%016lX)\n", mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
      }
      break;
    default: logfatal("Unimplemented COP0 instruction %d %d\n", mask_cop >> 4, mask_cop & 7);
  }
}