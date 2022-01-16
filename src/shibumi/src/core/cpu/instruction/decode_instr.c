#include <instruction/decode_instr.h>
#include <log.h>
#include <utils.h>
#include <access.h>

void exec(registers_t* regs, mem_t* mem, u32 instr) {
  u8 row = (instr >> 29) & 7;
  u8 column = (instr >> 26) & 7;
  switch(row) { // TODO: named constants for clearer code
    case 0:
      switch(column) {
        case 0: special(regs, mem, instr); break;
        case 1: regimm(regs, mem, instr); break;
        case 2: j(regs, instr); break;
        case 3: jal(regs, instr); break;
        case 4: b(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
        case 5: b(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
        case 6: b(regs, instr, regs->gpr[RS(instr)] <= 0); break;
        case 7: b(regs, instr, regs->gpr[RS(instr)] > 0); break;
      } break;
    case 1:
      switch(column) {
        case 0: addiu(regs, instr); break;
        case 1: addiu(regs, instr); break;
        case 2: slti(regs, instr); break;
        case 3: sltiu(regs, instr); break;
        case 4: andi(regs, instr); break;
        case 5: ori(regs, instr); break;
        case 6: xori(regs, instr); break;
        case 7: lui(regs, instr); break;
      } break;
    case 2:
      switch(column) {
        case 0 ... 2: {
          u8 row_cop = (instr >> 24) & 3;
          u8 column_cop = (instr >> 21) & 7;
          switch(row_cop) {
            case 0:
              switch(column_cop) {
                case 0: mfcz(regs, instr, column); break;
                case 2: cfcz(regs, instr, column); break;
                case 4: mtcz(regs, instr, column); break;
                case 6: ctcz(regs, instr, column); break;
                default:
                  logfatal("Unimplemented COP column %02X\n", column_cop);
                  break;
              } break;
            default:
              logfatal("Unimplemented COP row %02X\n", row_cop);
              break;
          } break;
        } break;
        case 4: bl(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
        case 5: bl(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
        case 6: bl(regs, instr, regs->gpr[RS(instr)] <= 0); break;
        case 7: bl(regs, instr, regs->gpr[RS(instr)] >= 0); break;
      } break;
    case 3:
      switch(column) {
        case 0: daddiu(regs, instr); break;
        case 1: daddiu(regs, instr); break;
        case 2: ldl(mem, regs, instr); break;
        case 3: ldr(mem, regs, instr); break;
      } break;
    case 4:
      switch(column) {
        case 0: lb(mem, regs, instr); break;
        case 1: lh(mem, regs, instr); break;
        case 2: lwl(mem, regs, instr); break;
        case 3: lw(mem, regs, instr); break;
        case 4: lbu(mem, regs, instr); break;
        case 5: lhu(mem, regs, instr); break;
        case 6: lwr(mem, regs, instr); break;
        case 7: lwu(mem, regs, instr); break;
      } break;
    case 5:
      switch(column) {
        case 0: sb(mem, regs, instr); break;
        case 1: sh(mem, regs, instr); break;
        case 2: swl(mem, regs, instr); break;
        case 3: sw(mem, regs, instr); break;
        case 4: sdl(mem, regs, instr); break;
        case 5: sdr(mem, regs, instr); break;
        case 6: swr(mem, regs, instr); break;
        case 7: break; // cache
      } break;
    case 6:
      switch(column) {
        case 0: ll(mem, regs, instr); break;
        case 4: lld(mem, regs, instr); break;
        case 7: ld(mem, regs, instr); break;
      } break;
    case 7:
      switch(column) {
        case 0: sc(mem, regs, instr); break;
        case 4: scd(mem, regs, instr); break;
        case 7: sd(mem, regs, instr); break;
      } break;
    default:
      logfatal("Unimplemented row %02X\n", row);
      break;
  }
}

void special(registers_t* regs, mem_t *mem, u32 instr) {
  u8 mask = (instr & 0x3F);
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
      logfatal("Unimplemented special %08X\n", instr);
      break;
  }
}

void regimm(registers_t* regs, mem_t *mem, u32 instr) {
  u8 mask = ((instr >> 16) & 0x1F);

  switch (mask) { // TODO: named constants for clearer code
    case 0x00: b(regs, instr, regs->gpr[RS(instr)] < 0); break;
    case 0x01: b(regs, instr, regs->gpr[RS(instr)] >= 0); break;
    case 0x03: bl(regs, instr, regs->gpr[RS(instr)] >= 0); break;
    case 0x11:
      regs->gpr[31] = regs->pc + 4;
      b(regs, instr, regs->gpr[RS(instr)] >= 0);
      break;
    default:
      logfatal("Unimplemented regimm %d %d\n", (instr >> 19) & 3, (instr >> 16) & 7);
      break;
  }
}