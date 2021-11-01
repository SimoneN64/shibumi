#include <instructions.h>
#include <utils.h>
#include <assert.h>

#define ze_imm(x) ((x) & 0xffff)
#define se_imm(x) ((s16)((x) & 0xFFFF))

void mtcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 0:
    ((s64*)&regs->cp0)[RD(instr)] = (u32)regs->gpr[RT(instr)];
    break;
  case 1:
    ((s64*)&regs->cp1)[RD(instr)] = (u32)regs->gpr[RT(instr)];
    break;
  default:
    logfatal("Invalid MTC%d", index);
  }
}

void mfcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 0:
    regs->gpr[RT(instr)] = ((s64*)&regs->cp0)[RD(instr)];
    break;
  case 1:
    regs->gpr[RT(instr)] = ((s64*)&regs->cp1)[RD(instr)];
    break;
  default:
    logfatal("Invalid MFC%d", index);
  }
}

void cfcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 1: {
    u8 rd = RD(instr);
    assert(rd == 0 || rd == 31);
    regs->gpr[RT(instr)] = ((s64*)&regs->cp1.fcr)[rd];
  } break;
  default:
    logfatal("Invalid CFC%d", index);
  }
}

void ctcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 1: {
    u8 rd = RD(instr);
    assert(rd == 0 || rd == 31);
    ((s64*)&regs->cp1.fcr)[rd] = regs->gpr[RT(instr)];
  } break;
  default:
    logfatal("Invalid CTC%d", index);
  }
}

void add(registers_t* regs, u32 instr) {
  s32 rs = regs->gpr[RS(instr)];
  s32 rt = regs->gpr[RT(instr)];
  s32 result = rs + rt;
  regs->gpr[RD(instr)] = (s64)result;
}

void addu(registers_t* regs, u32 instr) {
  s32 rs = regs->gpr[RS(instr)];
  s32 rt = regs->gpr[RT(instr)];
  s32 result = rs + rt;
  regs->gpr[RD(instr)] = (s64)result;
}

void addiu(registers_t* regs, u32 instr) {
  u32 reg = regs->gpr[RS(instr)];
  s16 imm = instr;
  s32 result = reg + imm;
  regs->gpr[RT(instr)] = (s64)result;
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

void divu(registers_t* regs, u32 instr) {
  u32 dividend = regs->gpr[RS(instr)];
  u32 divisor = regs->gpr[RT(instr)];
  if(divisor == 0) {
    regs->lo = 0xffffffffffffffff;
    regs->hi = (s32)dividend;
  } else {
    s32 quotient = dividend / divisor;
    s32 remainder = dividend % divisor;
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
    logfatal("Unaligned access that shouldn't have happened");
  }

  s16 value = read16(mem, address);
  regs->gpr[RT(instr)] = (s64)value;
}

void lw(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  s32 value = read32(mem, address);
  regs->gpr[RT(instr)] = (s64)value;
}

void ld(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  s64 value = read64(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lbu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  u8 value = read8(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lhu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 1) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  u16 value = read16(mem, address);
  regs->gpr[RT(instr)] = value;
}

void lwu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  u32 value = read32(mem, address);
  regs->gpr[RT(instr)] = value;
}

void sb(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  write8(mem, address, regs->gpr[RT(instr)]);
}

void sh(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 1) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  write16(mem, address, regs->gpr[RT(instr)]);
}

void sw(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }
  
  write32(mem, regs, address, regs->gpr[RT(instr)]);
}

void sd(mem_t* mem, registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)] + (s16)instr;
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }
  
  write64(mem, address, regs->gpr[RT(instr)]);
}

void ori(registers_t* regs, u32 instr) {
  u64 rs = regs->gpr[RS(instr)];
  u64 imm = instr & 0xffff;
  s64 result = imm | rs;
  regs->gpr[RT(instr)] = result;
}

void or_(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RS(instr)] | regs->gpr[RT(instr)];
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
  u16 imm = instr;
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] ^ imm;
}

void xor_(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RT(instr)] ^ regs->gpr[RS(instr)];
}

void andi(registers_t* regs, u32 instr) {
  s64 imm = ze_imm(instr);
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

void srl(registers_t* regs, u32 instr) {
  s32 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  s32 result = (u32)rt >> sa;
  regs->gpr[RD(instr)] = result;
}

void sra(registers_t* regs, u32 instr) {
  s32 rt = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  s32 result = rt >> sa;
  regs->gpr[RD(instr)] = (s64)result;
}

void sllv(registers_t* regs, u32 instr) {
  u8 amount = (regs->gpr[RS(instr)]) & 0x1F;
  u32 rt = regs->gpr[RT(instr)];
  s32 result = rt << amount;
  regs->gpr[RD(instr)] = (s64)result;
}

void srlv(registers_t* regs, u32 instr) {
  u8 amount = (regs->gpr[RS(instr)] & 0x1F);
  u32 rt = regs->gpr[RT(instr)];
  s32 result = rt >> amount;
  regs->gpr[RD(instr)] = (s64)result;
}

void j(registers_t* regs, u32 instr) {
  u32 target = (instr & 0x3ffffff) << 2;
  u32 address = (regs->old_pc & ~0xfffffff) | target;
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened (instr %08X) (addr: %08X)\n", instr, address);
  }
  
  branch(regs, true, address);
}

void jr(registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)];
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened (instr %08X) (addr: %08X)\n", instr, address);
  }
  
  branch(regs, true, address);
}

void subu(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u32 rs = regs->gpr[RS(instr)];
  u32 result = rs - rt;
  regs->gpr[RD(instr)] = (s64)((s32)result);
}

void multu(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u32 rs = regs->gpr[RS(instr)];
  u64 result = (u64)rt * (u64)rs;
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