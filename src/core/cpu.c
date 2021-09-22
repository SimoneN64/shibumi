#include <cpu.h>
#include <instructions.h>
#include <utils.h>

void init_cpu(cpu_t *cpu) {
  init_registers(&cpu->regs);
}

void step(cpu_t *cpu, mem_t *mem) {
  cpu->regs.gpr[0] = 0;
  u32 instruction = read32(mem, cpu->regs.pc);
  cpu->regs.old_pc = cpu->regs.pc;
  cpu->regs.pc = cpu->regs.next_pc;
  cpu->regs.next_pc += 4;
  exec(cpu, mem, instruction);
}

void exec(cpu_t* cpu, mem_t* mem, u32 instr) {
  u8 mask = (instr >> 26) & 0x3f;
  registers_t* regs = &cpu->regs;
  switch(mask) { // TODO: named constants for clearer code
  case 0x00: special(cpu, mem, instr); break;
  case 0x01: regimm(cpu, mem, instr); break;
  case 0x03: jal(regs, instr); break;
  case 0x04: b(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
  case 0x05: b(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
  case 0x08: 
  case 0x09: addiu(regs, instr);
  case 0x0A: slti(regs, instr);
  case 0x14: bl(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
  case 0x15: bl(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
  case 0x16: bl(regs, instr, regs->gpr[RS(instr)] <= 0); break;
  case 0x0C: andi(regs, instr); break;
  case 0x0D: ori(regs, instr); break;
  case 0x0E: xori(regs, instr); break;
  case 0x0F: lui(regs, instr); break;
  case 0x10: mtcz(regs, instr, ((instr >> 26) & 3)); break;
  case 0x23: lw(mem, regs, instr); break;
  case 0x24: lbu(mem, regs, instr); break;
  case 0x28: sb(mem, regs, instr); break;
  case 0x2B: sw(mem, regs, instr); break;
  case 0x2F: break;
  default: logfatal("[CPU ERR] Unimplemented instruction %02X, PC: %08X%08X\n", mask, (u32)(regs->old_pc >> 32), (u32)regs->old_pc);
  }
}

void special(cpu_t *cpu, mem_t *mem, u32 instr) {
  u8 mask = (instr & 0x3F);
  registers_t* regs = &cpu->regs;
  switch (mask) { // TODO: named constants for clearer code
    case 0:
    if (instr != 0) {
      sll(regs, instr);
    }
    break;
    case 0x02: srl(regs, instr); break;
    case 0x04: sllv(regs, instr); break;
    case 0x06: srlv(regs, instr); break;
    case 0x08: jr(regs, instr); break;
    case 0x10: mfhi(regs, instr); break;
    case 0x12: mflo(regs, instr); break;
    case 0x19: multu(regs, instr); break;
    case 0x20: add(regs, instr); break;
    case 0x21: addu(regs, instr); break;
    case 0x23: subu(regs, instr); break;
    case 0x24: and(regs, instr); break;
    case 0x25: or(regs, instr); break;
    case 0x26: xor(regs, instr); break;
    case 0x2A: slt(regs, instr); break;
    case 0x2B: sltu(regs, instr); break;
    default: logfatal("[CPU ERR] Unimplemented special instruction %02X, PC: %08X%08X\n", mask, (u32)(regs->old_pc >> 32), (u32)regs->old_pc);
  }
}

void regimm(cpu_t *cpu, mem_t *mem, u32 instr) {
  u8 mask = ((instr >> 16) & 0x1F);
  registers_t* regs = &cpu->regs;

  switch (mask) { // TODO: named constants for clearer code
  case 0x03: bl(regs, instr, regs->gpr[RS(instr)] >= 0); break;
  case 0x11:
    regs->gpr[31] = regs->pc + 4;
    b(regs, instr, regs->gpr[RS(instr)] >= 0);
    break;
  default: logfatal("[CPU ERR] Unimplemented regimm instruction %02X, PC: %08X%08X\n", mask, (u32)(regs->old_pc >> 32), (u32)regs->old_pc);
  }
}