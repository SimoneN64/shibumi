#include <instruction/decode_instr.h>
#include <log.h>
#include <access.h>

void exec(registers_t* regs, mem_t* mem, u32 instr) {
  u8 mask = (instr >> 26) & 0x3f;
  // 00rr_rccc
  switch(mask) { // TODO: named constants for clearer code
    case 0x00: special(regs, instr); break;
    case 0x01: regimm(regs, instr); break;
    case 0x02: j(regs, instr); break;
    case 0x03: jal(regs, instr); break;
    case 0x04: b(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
    case 0x05: b(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
    case 0x06: b(regs, instr, regs->gpr[RS(instr)] <= 0); break;
    case 0x07: b(regs, instr, regs->gpr[RS(instr)] > 0); break;
    case 0x08: addiu(regs, instr); break;
    case 0x09: addiu(regs, instr); break;
    case 0x0A: slti(regs, instr); break;
    case 0x0B: sltiu(regs, instr); break;
    case 0x0C: andi(regs, instr); break;
    case 0x0D: ori(regs, instr); break;
    case 0x0E: xori(regs, instr); break;
    case 0x0F: lui(regs, instr); break;
    case 0x10 ... 0x11: {
      u8 mask_cop = (instr >> 21) & 0x1F;
      switch(mask_cop) {
        case 0x00: mfcz(regs, instr, mask & 3); break;
        case 0x02: cfcz(regs, instr, mask & 3); break;
        case 0x04: mtcz(regs, instr, mask & 3); break;
        case 0x06: ctcz(regs, instr, mask & 3); break;
        case 0x10 ... 0x1F: {
          u8 mask_cop2 = instr & 0x3F;
          switch(mask_cop2) {
            case 0x02: break;
            case 0x18: eret(regs); break;
            default: logfatal("Unimplemented COP%d CO instruction %d %d (%08X) (%016lX)", mask & 3, mask_cop2 >> 3, mask_cop2 & 7, instr, regs->old_pc);
          }
        } break;
        default: logfatal("Unimplemented COP%d instruction %d %d", mask & 3, mask_cop >> 4, mask_cop & 7);
      }
    } break;
    case 0x14: bl(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
    case 0x15: bl(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
    case 0x16: bl(regs, instr, regs->gpr[RS(instr)] <= 0); break;
    case 0x17: bl(regs, instr, regs->gpr[RS(instr)] > 0); break;
    case 0x18: daddiu(regs, instr); break;
    case 0x19: daddiu(regs, instr); break;
    case 0x1A: ldl(mem, regs, instr); break;
    case 0x1B: ldr(mem, regs, instr); break;
    case 0x20: lb(mem, regs, instr); break;
    case 0x21: lh(mem, regs, instr); break;
    case 0x22: lwl(mem, regs, instr); break;
    case 0x23: lw(mem, regs, instr); break;
    case 0x24: lbu(mem, regs, instr); break;
    case 0x25: lhu(mem, regs, instr); break;
    case 0x26: lwr(mem, regs, instr); break;
    case 0x27: lwu(mem, regs, instr); break;
    case 0x28: sb(mem, regs, instr); break;
    case 0x29: sh(mem, regs, instr); break;
    case 0x2A: swl(mem, regs, instr); break;
    case 0x2B: sw(mem, regs, instr); break;
    case 0x2C: sdl(mem, regs, instr); break;
    case 0x2D: sdr(mem, regs, instr); break;
    case 0x2E: swr(mem, regs, instr); break;
    case 0x2F: break; // cache
    case 0x30: ll(mem, regs, instr); break;
    case 0x34: lld(mem, regs, instr); break;
    case 0x37: ld(mem, regs, instr); break;
    case 0x38: sc(mem, regs, instr); break;
    case 0x3C: scd(mem, regs, instr); break;
    case 0x3F: sd(mem, regs, instr); break;
    default:
      logfatal("Unimplemented instruction %02X %d\n", mask, mask & 7);
  }
}

void special(registers_t* regs, u32 instr) {
  u8 mask = (instr & 0x3F);
  // 00rr_rccc
  switch (mask) { // TODO: named constants for clearer code
    case 0:
    if (instr != 0) {
      sll(regs, instr);
    }
    break;
    case 0x02: srl(regs, instr); break;
    case 0x03: sra(regs, instr); break;
    case 0x04: sllv(regs, instr); break;
    case 0x06: srlv(regs, instr); break;
    case 0x07: srav(regs, instr); break;
    case 0x08: jr(regs, instr); break;
    case 0x09: jalr(regs, instr); break;
    case 0x0F: break;
    case 0x10: mfhi(regs, instr); break;
    case 0x11: mthi(regs, instr); break;
    case 0x12: mflo(regs, instr); break;
    case 0x13: mtlo(regs, instr); break;
    case 0x14: dsllv(regs, instr); break;
    case 0x16: dsrlv(regs, instr); break;
    case 0x17: dsrav(regs, instr); break;
    case 0x18: mult(regs, instr); break;
    case 0x19: multu(regs, instr); break;
    case 0x1A: div_(regs, instr); break;
    case 0x1B: divu(regs, instr); break;
    case 0x1C: dmult(regs, instr); break;
    case 0x1D: dmultu(regs, instr); break;
    case 0x1E: ddiv(regs, instr); break;
    case 0x1F: ddivu(regs, instr); break;
    case 0x20: add(regs, instr); break;
    case 0x21: addu(regs, instr); break;
    case 0x22: sub(regs, instr); break;
    case 0x23: subu(regs, instr); break;
    case 0x24: and_(regs, instr); break;
    case 0x25: or_(regs, instr); break;
    case 0x26: xor_(regs, instr); break;
    case 0x27: nor(regs, instr); break;
    case 0x2A: slt(regs, instr); break;
    case 0x2B: sltu(regs, instr); break;
    case 0x2C: daddu(regs, instr); break;
    case 0x2D: daddu(regs, instr); break;
    case 0x2E: dsub(regs, instr); break;
    case 0x2F: dsubu(regs, instr); break;
    case 0x38: dsll(regs, instr); break;
    case 0x3A: dsrl(regs, instr); break;
    case 0x3B: dsra(regs, instr); break;
    case 0x3C: dsll32(regs, instr); break;
    case 0x3E: dsrl32(regs, instr); break;
    case 0x3F: dsra32(regs, instr); break;
    default:
      logfatal("Unimplemented special %d %d\n", (mask >> 3) & 7, mask & 7);
  }
}

void regimm(registers_t* regs, u32 instr) {
  u8 mask = ((instr >> 16) & 0x1F);
  // 000r_rccc
  switch (mask) { // TODO: named constants for clearer code
    case 0x00: b(regs, instr, regs->gpr[RS(instr)] < 0); break;
    case 0x01: b(regs, instr, regs->gpr[RS(instr)] >= 0); break;
    case 0x02: bl(regs, instr, regs->gpr[RS(instr)] < 0); break;
    case 0x03: bl(regs, instr, regs->gpr[RS(instr)] >= 0); break;
    case 0x10:
      regs->gpr[31] = regs->pc + 4;
      b(regs, instr, regs->gpr[RS(instr)] < 0);
      break;
    case 0x11:
      regs->gpr[31] = regs->pc + 4;
      b(regs, instr, regs->gpr[RS(instr)] >= 0);
      break;
    case 0x12:
      regs->gpr[31] = regs->pc + 4;
      bl(regs, instr, regs->gpr[RS(instr)] < 0);
      break;
    case 0x13:
      regs->gpr[31] = regs->pc + 4;
      bl(regs, instr, regs->gpr[RS(instr)] >= 0);
      break;
    default:
      logfatal("Unimplemented regimm %d %d\n", (instr >> 19) & 3, (instr >> 16) & 7);
  }
}