#include <instructions.h>
#include <instruction/decode_instr.h>
#include <utils.h>
#include <assert.h>

#define ze_imm(x) ((x) & 0xffff)
#define se_imm(x) ((s16)((x) & 0xFFFF))

void mtcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 0:
    set_cop0_reg_word(&regs->cp0, RD(instr), regs->gpr[RT(instr)]);
    break;
  case 1:
    set_cop1_reg_word(&regs->cp1, &regs->cp0, RD(instr), regs->gpr[RT(instr)]);
    break;
  default:
    log_(FATAL, "Invalid MTC%d", index);
  }
}

void mfcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 0:
    regs->gpr[RT(instr)] = (s32)get_cop0_reg_word(&regs->cp0, RD(instr));
    break;
  case 1:
    regs->gpr[RT(instr)] = (s32)get_cop1_reg_word(&regs->cp1, &regs->cp0, RD(instr));
    break;
  default:
    log_(FATAL, "Invalid MFC%d", index);
  }
}

void cfcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 1: {
    u8 rd = RD(instr);
    s32 val = 0;
    switch(rd) {
      case 0: val = regs->cp1.fcr0; break;
      case 31: val = regs->cp1.fcr31.raw; break;
      default: log_(FATAL, "Undefined CFC1 with rd != 0 or 31\n");
    }
    regs->gpr[RT(instr)] = val;
  } break;
  default:
    log_(FATAL, "Invalid CFC%d", index);
  }
}

void ctcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 1: {
    u8 rd = RD(instr);
    u32 val = regs->gpr[RT(instr)];
    switch(rd) {
      case 0: log_(FATAL, "CTC1 attempt to write to FCR0 which is read only!\n");
      case 31: {
        val &= 0x183ffff;
        regs->cp1.fcr31.raw = val;
      } break;
      default: log_(FATAL, "Undefined CTC1 with rd != 0 or 31\n");
    }
  } break;
  default:
    log_(FATAL, "Invalid CTC%d", index);
  }
}

void add(registers_t* regs, u32 instr) {
  s32 rs = regs->gpr[RS(instr)];
  s32 rt = regs->gpr[RT(instr)];
  s32 result = rs + rt;
  regs->gpr[RD(instr)] = result;
}

void addu(registers_t* regs, u32 instr) {
  s32 rs = regs->gpr[RS(instr)];
  s32 rt = regs->gpr[RT(instr)];
  s32 result = rs + rt;
  regs->gpr[RD(instr)] = result;
}

void addiu(registers_t* regs, u32 instr) {
  u32 reg = regs->gpr[RS(instr)];
  s16 imm = instr;
  s32 result = reg + imm;
  regs->gpr[RT(instr)] = result;
}

void daddiu(registers_t* regs, u32 instr) {
  s16 addend1 = instr;
  s64 addend2 = regs->gpr[RS(instr)];
  regs->gpr[RT(instr)] = addend1 + addend2;
}

void daddu(registers_t* regs, u32 instr) {
  s64 addend1 = regs->gpr[RS(instr)];
  s64 addend2 = regs->gpr[RT(instr)];
  regs->gpr[RD(instr)] = addend1 + addend2;
}

void div_(registers_t* regs, u32 instr) {
  s64 dividend = (s32)regs->gpr[RS(instr)];
  s64 divisor = (s32)regs->gpr[RT(instr)];
  
  if(divisor == 0) {
    regs->hi = dividend;
    if(dividend >= 0) {
      regs->lo = -1;
    } else {
      regs->lo = 1;
    }
  } else {
    s64 quotient = dividend / divisor;
    s64 remainder = dividend % divisor;
    regs->lo = quotient;
    regs->hi = remainder;
  }
}

void divu(registers_t* regs, u32 instr) {
  u32 dividend = regs->gpr[RS(instr)];
  u32 divisor = regs->gpr[RT(instr)];
  if(divisor == 0) {
    regs->lo = -1;
    regs->hi = (s32)dividend;
  } else {
    s32 quotient = dividend / divisor;
    s32 remainder = dividend % divisor;
    regs->lo = quotient;
    regs->hi = remainder;
  }
}

void ddiv(registers_t* regs, u32 instr) {
  s64 dividend = regs->gpr[RS(instr)];
  s64 divisor = regs->gpr[RT(instr)];
  if (dividend == 0x8000000000000000 && divisor == 0xFFFFFFFFFFFFFFFF) {
    regs->lo = dividend;
    regs->hi = 0;
  } else if(divisor == 0) {
    regs->hi = dividend;
    if(dividend >= 0) {
      regs->lo = -1;
    } else {
      regs->lo = 1;
    }
  } else {
    s64 quotient = dividend / divisor;
    s64 remainder = dividend % divisor;
    regs->lo = quotient;
    regs->hi = remainder;
  }
}

void ddivu(registers_t* regs, u32 instr) {
  u64 dividend = regs->gpr[RS(instr)];
  u64 divisor = regs->gpr[RT(instr)];
  if(divisor == 0) {
    regs->lo = -1;
    regs->hi = dividend;
  } else {
    u64 quotient = dividend / divisor;
    u64 remainder = dividend % divisor;
    regs->lo = quotient;
    regs->hi = remainder;
  }
}

void branch(registers_t* regs, bool cond, s64 address) {
  if (cond) {
    regs->next_pc = address;
  }
}

void branch_likely(registers_t* regs, bool cond, s64 address) {
  if (cond) {
    regs->next_pc = address;
  } else {
    set_pc(regs, regs->pc + 4);
  }
}

void b(registers_t* regs, u32 instr, bool cond) {
  s64 offset = se_imm((s64)instr) << 2;
  s64 address = regs->pc + offset;
  branch(regs, cond, address);
}

void bl(registers_t* regs, u32 instr, bool cond) {
  s64 offset = se_imm((s64)instr) << 2;
  s64 address = regs->pc + offset;
  branch_likely(regs, cond, address);
}

void lui(registers_t* regs, u32 instr) {
  s64 val = (s16)instr;
  val *= 65536;
  regs->gpr[RT(instr)] = val;
}

void lb(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  s8 value = read8(mem, address);
  regs->gpr[RT(instr)] = (s64)value;
}

void lh(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 1) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  s16 value = read16(mem, address);
  regs->gpr[RT(instr)] = (s64)value;
}

void lw(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  s32 value = read32(mem, address);
  regs->gpr[RT(instr)] = value;
}

void ll(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  regs->LLBit = true;
  regs->cp0.LLAddr = address;

  s32 value = read32(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lwl(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  u32 shift = 8 * ((address ^ 0) & 3);
  u32 mask = 0xFFFFFFFF << shift;
  u32 data = read32(mem, address & ~3);
  s64 rt = regs->gpr[RT(instr)];
  s32 result = (rt & ~mask) | (data << shift);
  regs->gpr[RT(instr)] = result;
}

void lwr(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  u32 shift = 8 * ((address ^ 3) & 3);
  u32 mask = 0xFFFFFFFF >> shift;
  u32 data = read32(mem, address & ~3);
  s64 rt = regs->gpr[RT(instr)];
  s32 result = (rt & ~mask) | (data >> shift);
  regs->gpr[RT(instr)] = result;
}

void ld(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 7) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  s64 value = read64(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lld(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 7) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  regs->LLBit = true;
  regs->cp0.LLAddr = address;

  s64 value = read64(mem, address);
  regs->gpr[RT(instr)] = value;
}

void ldl(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  s32 shift = 8 * ((address ^ 0) & 7);
  u64 mask = 0xFFFFFFFFFFFFFFFF << shift;
  u64 data = read64(mem, address & ~7);
  s64 rt = regs->gpr[RT(instr)];
  s64 result = (rt & ~mask) | (data << shift);
  regs->gpr[RT(instr)] = result;
}

void ldr(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  s32 shift = 8 * ((address ^ 7) & 7);
  u64 mask = 0xFFFFFFFFFFFFFFFF >> shift;
  u64 data = read64(mem, address & ~7);
  s64 rt = regs->gpr[RT(instr)];
  s64 result = (rt & ~mask) | (data >> shift);
  regs->gpr[RT(instr)] = result;
}

void lbu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  u8 value = read8(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lhu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 1) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  u16 value = read16(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lwu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  u32 value = read32(mem, address);
  regs->gpr[RT(instr)] = value;
}

void sb(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  write8(mem, address, regs->gpr[RT(instr)]);
}

void sc(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }
  
  if(regs->LLBit) {
    write32(mem, regs, address, regs->gpr[RT(instr)]);
  }

  regs->gpr[RT(instr)] = (u64)regs->LLBit;
  regs->LLBit = false;
}

void scd(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 7) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }
  
  if(regs->LLBit) {
    write64(mem, address, regs->gpr[RT(instr)]);
  }

  regs->gpr[RT(instr)] = (u64)regs->LLBit;
  regs->LLBit = false;
}

void sh(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 1) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }

  write16(mem, address, regs->gpr[RT(instr)]);
}

void sw(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }
  
  write32(mem, regs, address, regs->gpr[RT(instr)]);
}

void sd(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 7) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened\n");
  }
  
  write64(mem, address, regs->gpr[RT(instr)]);
}

void sdl(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  s32 shift = 8 * ((address ^ 0) & 7);
  u64 mask = 0xFFFFFFFFFFFFFFFF >> shift;
  u64 data = read64(mem, address & ~7);
  s64 rt = regs->gpr[RT(instr)];
  write64(mem, address & ~7, (data & ~mask) | (rt >> shift));
}

void sdr(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  s32 shift = 8 * ((address ^ 7) & 7);
  u64 mask = 0xFFFFFFFFFFFFFFFF << shift;
  u64 data = read64(mem, address & ~7);
  s64 rt = regs->gpr[RT(instr)];
  write64(mem, address & ~7, (data & ~mask) | (rt << shift));
}

void swl(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  u32 shift = 8 * ((address ^ 0) & 3);
  u32 mask = 0xFFFFFFFF >> shift;
  u32 data = read32(mem, address & ~3);
  u32 rt = regs->gpr[RT(instr)];
  write32(mem, regs, address & ~3, (data & ~mask) | (rt >> shift));
}

void swr(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  u32 shift = 8 * ((address ^ 3) & 3);
  u32 mask = 0xFFFFFFFF << shift;
  u32 data = read32(mem, address & ~3);
  u32 rt = regs->gpr[RT(instr)];
  write32(mem, regs, address & ~3, (data & ~mask) | (rt << shift));
}

void ori(registers_t* regs, u32 instr) {
  s64 imm = (u16)instr;
  s64 result = imm | regs->gpr[RS(instr)];
  regs->gpr[RT(instr)] = result;
}

void or_(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RS(instr)] | regs->gpr[RT(instr)];
}

void nor(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = ~(regs->gpr[RS(instr)] | regs->gpr[RT(instr)]);
}

void jal(registers_t* regs, u32 instr) {
  regs->gpr[31] = regs->pc + 4;
  s64 target = (instr & 0x3ffffff) << 2;
  s64 combined = (regs->old_pc & ~0xfffffff) | target;
  branch(regs, true, combined);
}

void jalr(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->pc + 4;
  branch(regs, true, regs->gpr[RS(instr)]);
}

void slti(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] < se_imm((s64)instr);
}

void sltiu(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = (u64)regs->gpr[RS(instr)] < se_imm((s64)instr);
}

void slt(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RS(instr)] < regs->gpr[RT(instr)];
}

void sltu(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = (u64)regs->gpr[RS(instr)] < (u64)regs->gpr[RT(instr)];
}

void xori(registers_t* regs, u32 instr) {
  s64 imm = (u16)instr;
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] ^ imm;
}

void xor_(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RT(instr)] ^ regs->gpr[RS(instr)];
}

void andi(registers_t* regs, u32 instr) {
  s64 imm = (u16)instr;
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] & imm;
}

void and_(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RS(instr)] & regs->gpr[RT(instr)];
}

void sll(registers_t* regs, u32 instr) {
  u8 sa = ((instr >> 6) & 0x1f);
  s32 result = regs->gpr[RT(instr)] << sa;
  regs->gpr[RD(instr)] = (s64)result;
}

void sllv(registers_t* regs, u32 instr) {
  u8 sa = (regs->gpr[RS(instr)]) & 0x1F;
  u32 rt = regs->gpr[RT(instr)];
  s32 result = rt << sa;
  regs->gpr[RD(instr)] = (s64)result;
}

void dsll32(registers_t* regs, u32 instr) {
  u8 sa = ((instr >> 6) & 0x1f);
  s64 result = regs->gpr[RT(instr)] << (sa + 32);
  regs->gpr[RD(instr)] = result;
}

void dsll(registers_t* regs, u32 instr) {
  u8 sa = ((instr >> 6) & 0x1f);
  s64 result = regs->gpr[RT(instr)] << sa;
  regs->gpr[RD(instr)] = result;
}

void dsllv(registers_t* regs, u32 instr) {
  s64 sa = regs->gpr[RS(instr)] & 63;
  s64 result = regs->gpr[RT(instr)] << sa;
  regs->gpr[RD(instr)] = result;
}

void srl(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  u32 result = rt >> sa;
  regs->gpr[RD(instr)] = (s32)result;
}

void srlv(registers_t* regs, u32 instr) {
  u8 sa = (regs->gpr[RS(instr)] & 0x1F);
  u32 rt = regs->gpr[RT(instr)];
  s32 result = rt >> sa;
  regs->gpr[RD(instr)] = (s64)result;
}

void dsrl(registers_t* regs, u32 instr) {
  u64 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  u64 result = rt >> sa;
  regs->gpr[RD(instr)] = result;
}

void dsrlv(registers_t* regs, u32 instr) {
  u8 amount = (regs->gpr[RS(instr)] & 63);
  u64 rt = regs->gpr[RT(instr)];
  u64 result = rt >> amount;
  regs->gpr[RD(instr)] = result;
}

void dsrl32(registers_t* regs, u32 instr) {
  u64 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  u64 result = rt >> (sa + 32);
  regs->gpr[RD(instr)] = result;
}

void sra(registers_t* regs, u32 instr) {
  s32 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  s32 result = rt >> sa;
  regs->gpr[RD(instr)] = result;
}

void srav(registers_t* regs, u32 instr) {
  s32 rt = regs->gpr[RT(instr)];
  s32 rs = regs->gpr[RS(instr)];
  u8 sa = rs & 0x1f;
  s32 result = rt >> sa;
  regs->gpr[RD(instr)] = result;
}

void dsra(registers_t* regs, u32 instr) {
  s64 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  s64 result = rt >> sa;
  regs->gpr[RD(instr)] = result;
}

void dsrav(registers_t* regs, u32 instr) {
  s64 rt = regs->gpr[RT(instr)];
  s64 rs = regs->gpr[RS(instr)];
  s64 sa = rs & 63;
  s64 result = rt >> sa;
  regs->gpr[RD(instr)] = result;
}

void dsra32(registers_t* regs, u32 instr) {
  s64 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  s64 result = rt >> (sa + 32);
  regs->gpr[RD(instr)] = result;
}

void j(registers_t* regs, u32 instr) {
  u32 target = (instr & 0x3ffffff) << 2;
  u32 address = (regs->old_pc & ~0xfffffff) | target;
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened (instr: %08X) (addr: %08X)\n", instr, address);
  }
  
  branch(regs, true, address);
}

void jr(registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)];
  if ((address & 3) != 0) {
    log_(FATAL, "Unaligned access that shouldn't have happened (instr %08X) (addr: %08X)\n", instr, address);
  }
  
  branch(regs, true, address);
}

void dsub(registers_t* regs, u32 instr) {
  s64 rt = regs->gpr[RT(instr)];
  s64 rs = regs->gpr[RS(instr)];
  s64 result = rs - rt;
  regs->gpr[RD(instr)] = result;
}

void dsubu(registers_t* regs, u32 instr) {
  u64 rt = regs->gpr[RT(instr)];
  u64 rs = regs->gpr[RS(instr)];
  u64 result = rs - rt;
  regs->gpr[RD(instr)] = result;
}

void sub(registers_t* regs, u32 instr) {
  s32 rt = regs->gpr[RT(instr)];
  s32 rs = regs->gpr[RS(instr)];
  s32 result = rs - rt;
  regs->gpr[RD(instr)] = (s64)result;
}

void subu(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u32 rs = regs->gpr[RS(instr)];
  u32 result = rs - rt;
  regs->gpr[RD(instr)] = (s64)((s32)result);
}

void dmultu(registers_t* regs, u32 instr) {
  u64 rt = regs->gpr[RT(instr)];
  u64 rs = regs->gpr[RS(instr)];
  u128 result = (u128)rt * (u128)rs;
  regs->lo = (s64)(result & 0xFFFFFFFFFFFFFFFF);
  regs->hi = (s64)(result >> 64);
}

void dmult(registers_t* regs, u32 instr) {
  s64 rt = regs->gpr[RT(instr)];
  s64 rs = regs->gpr[RS(instr)];
  s128 result = (s128)rt * (s128)rs;
  regs->lo = result & 0xFFFFFFFFFFFFFFFF;
  regs->hi = result >> 64;
}

void multu(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u32 rs = regs->gpr[RS(instr)];
  u64 result = (u64)rt * (u64)rs;
  regs->lo = (s64)((s32)result);
  regs->hi = (s64)((s32)(result >> 32));
}

void mult(registers_t* regs, u32 instr) {
  s32 rt = regs->gpr[RT(instr)];
  s32 rs = regs->gpr[RS(instr)];
  s64 result = (s64)rt * (s64)rs;
  regs->lo = (s64)((s32)result);
  regs->hi = (s64)((s32)(result >> 32));
}

void mflo(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->lo;
}

void mfhi(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->hi;
}

void mtlo(registers_t* regs, u32 instr) {
  regs->lo = regs->gpr[RS(instr)];
}

void mthi(registers_t* regs, u32 instr) {
  regs->hi = regs->gpr[RS(instr)];
}