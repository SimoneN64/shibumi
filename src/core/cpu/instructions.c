#include <instructions.h>
#include <utils.h>

#define ze_imm(x) ((x) & 0xffff)

static inline s64 se_imm(u32 instr) {
  return (s64)((s16)(instr & 0xffff));
}

void mtcz(registers_t* regs, u32 instr, u8 index) {
  switch(index) {
  case 0:
    regs->cp0[RD(instr)] = regs->gpr[RT(instr)];
    break;
  case 1:
    logfatal("Unimplemented MTC1");
    break;
  default:
    logfatal("Invalid MTC%d", index);
  }
}

void lui(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = se_imm(instr) << 16;
}

void addiu(registers_t* regs, u32 instr) {
  u32 reg = regs->gpr[RS(instr)];
  u32 imm = se_imm(instr);
  regs->gpr[RT(instr)] = (s64)((s32)(reg + imm));
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
  s64 offset = se_imm(instr) << 2;
  s64 address = regs->pc + offset;
  branch(regs, cond, address);
}

void bl(registers_t* regs, u32 instr, bool cond) {
  s64 offset = se_imm(instr) << 2;
  s64 address = regs->pc + offset;
  branch_likely(regs, cond, address);
}

void sw(mem_t* mem, registers_t* regs, u32 instr) {
  u32 rs = regs->gpr[RS(instr)];
  s64 address = rs + se_imm(instr);
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  u32 reg = regs->gpr[RT(instr)];
  write32(mem, address, reg);
}

void lw(mem_t* mem, registers_t* regs, u32 instr) {
  u32 rs = regs->gpr[RS(instr)];
  u32 address = rs + se_imm(instr);
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }

  regs->gpr[RT(instr)] = (s64)((s32)read32(mem, address));
}

void sb(mem_t* mem, registers_t* regs, u32 instr) {
  u32 rs = regs->gpr[RS(instr)];
  u32 address =  rs + se_imm(instr);
  write8(mem, address, regs->gpr[RT(instr)]);
}

void lbu(mem_t* mem, registers_t* regs, u32 instr) {
  u32 rs = regs->gpr[RS(instr)];
  u32 address =  rs + se_imm(instr);
  regs->gpr[RT(instr)] = read8(mem, address);
}

void ori(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] | ze_imm(instr);
}

void or(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RS(instr)] | regs->gpr[RT(instr)];
}

void jal(registers_t* regs, u32 instr) {
  regs->gpr[31] = regs->pc + 4;
  u32 target = (instr & 0x3ffffff) << 2;
  u32 combined = ((regs->old_pc) & 0xF0000000) | target;
  branch(regs, true, combined);
}

void slti(registers_t* regs, u32 instr) {
  if (regs->gpr[RS(instr)] < se_imm(instr)) {
    regs->gpr[RT(instr)] = 1;
  } else {
    regs->gpr[RT(instr)] = 0;
  }
}

void slt(registers_t* regs, u32 instr) {
  if (regs->gpr[RS(instr)] < regs->gpr[RT(instr)]) {
    regs->gpr[RD(instr)] = 1;
  } else {
    regs->gpr[RD(instr)] = 0;
  }
}

void sltu(registers_t* regs, u32 instr) {
  if ((u64)regs->gpr[RS(instr)] < (u64)regs->gpr[RT(instr)]) {
    regs->gpr[RD(instr)] = 1;
  } else {
    regs->gpr[RD(instr)] = 0;
  }
}

void srlv(registers_t* regs, u32 instr) {
  u8 amount = (regs->gpr[RS(instr)] & 0x1F);
  u32 rt = regs->gpr[RT(instr)];
  u32 result = rt >> amount;
  regs->gpr[RD(instr)] = (s64)((s32)result);
}

void sllv(registers_t* regs, u32 instr) {
  u8 amount = (regs->gpr[RS(instr)]) & 0x1F;
  u32 rt = regs->gpr[RT(instr)];
  u32 result = rt << amount;
  regs->gpr[RD(instr)] = (s64)((s32)result);
}

void xori(registers_t* regs, u32 instr) {
  s64 imm = ze_imm(instr);
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] ^ imm;
}

void xor(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RT(instr)] ^ regs->gpr[RS(instr)];
}

void andi(registers_t* regs, u32 instr) {
  s64 imm = ze_imm(instr);
  regs->gpr[RT(instr)] = regs->gpr[RS(instr)] & imm;
}

void and(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->gpr[RS(instr)] & regs->gpr[RT(instr)];
}

void sll(registers_t* regs, u32 instr) {
  u32 low = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  regs->gpr[RD(instr)] = (s64)((s32)(low << sa));
}

void srl(registers_t* regs, u32 instr) {
  u32 low = regs->gpr[RT(instr)];
  u8 sa = ((instr >> 6) & 0x1f);
  regs->gpr[RD(instr)] = (s64)((s32)(low >> sa));
}

void jr(registers_t* regs, u32 instr) {
  u32 address = regs->gpr[RS(instr)];
  if ((address & 3) != 0) {
    logfatal("Unaligned access that shouldn't have happened");
  }
  
  branch(regs, true, address);
}

void subu(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u32 result = rt - (u32)regs->gpr[RS(instr)];
  regs->gpr[RD(instr)] = (s64)((s32)result);
}

void multu(registers_t* regs, u32 instr) {
  u64 rt = regs->gpr[RT(instr)];
  u64 result = rt * (u32)regs->gpr[RS(instr)];
  regs->lo = (s64)((s32)result);
  regs->hi = (s64)((s32)(result >> 32));
}

void mflo(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->lo;
}

void mfhi(registers_t* regs, u32 instr) {
  regs->gpr[RD(instr)] = regs->hi;
}

void addu(registers_t* regs, u32 instr) {
  u32 rt = regs->gpr[RT(instr)];
  u32 result = rt + regs->gpr[RS(instr)];
  regs->gpr[RD(instr)] = (s64)((s32)result);
}

void add(registers_t* regs, u32 instr) {
  u32 rs = regs->gpr[RS(instr)];
  u32 sum = rs + regs->gpr[RT(instr)];
  regs->gpr[RD(instr)] = (s64)((s32)sum);
}