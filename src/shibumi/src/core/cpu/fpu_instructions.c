#include <fpu_instructions.h>
#include <log.h>
#include <math.h>

void absd(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  set_cop1_reg_double(&regs->cp1, &regs->cp0, FD(instr), fabs(fs));
}

void abss(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  set_cop1_reg_float(&regs->cp1, &regs->cp0, FD(instr), fabsf(fs));
}

void absw(registers_t* regs, u32 instr) {
  s32 fs = (s32)get_cop1_reg_word(&regs->cp1, &regs->cp0, FS(instr));
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FD(instr), abs(fs));
}

void absl(registers_t* regs, u32 instr) {
  s64 fs = (s64)get_cop1_reg_dword(&regs->cp1, &regs->cp0, FS(instr));
  set_cop1_reg_dword(&regs->cp1, &regs->cp0, FD(instr), labs(fs));
}

void cfc1(registers_t* regs, u32 instr) {
  u8 fd = FD(instr);
  s32 val = 0;
  switch(fd) {
    case 0: val = (s32)regs->cp1.fcr0; break;
    case 31: val = (s32)regs->cp1.fcr31.raw; break;
    default: logfatal("Undefined CFC1 with rd != 0 or 31\n");
  }
  regs->gpr[RT(instr)] = val;
}

void ctc1(registers_t* regs, u32 instr) {
  u8 fs = FS(instr);
  u32 val = regs->gpr[RT(instr)];
  switch(fs) {
    case 0: logfatal("CTC1 attempt to write to FCR0 which is read only!\n");
    case 31: {
      val &= 0x183ffff;
      regs->cp1.fcr31.raw = val;
    } break;
    default: logfatal("Undefined CTC1 with rd != 0 or 31\n");
  }
}

void cvtds(registers_t* regs, u32 instr) {
  set_cop1_reg_double(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_reg_float(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtsd(registers_t* regs, u32 instr) {
  set_cop1_reg_float(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_reg_double(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtwd(registers_t* regs, u32 instr) {
  set_cop1_reg_word(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_reg_double(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtws(registers_t* regs, u32 instr) {
  set_cop1_reg_word(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_reg_float(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtls(registers_t* regs, u32 instr) {
  set_cop1_reg_dword(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_reg_float(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtsl(registers_t* regs, u32 instr) {
  set_cop1_reg_float(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    (s64)get_cop1_reg_dword(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtdw(registers_t* regs, u32 instr) {
  set_cop1_reg_double(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    (s32)get_cop1_reg_word(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtsw(registers_t* regs, u32 instr) {
  set_cop1_reg_float(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    (s32)get_cop1_reg_word(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtdl(registers_t* regs, u32 instr) {
  set_cop1_reg_double(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    (s64)get_cop1_reg_dword(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void cvtld(registers_t* regs, u32 instr) {
  set_cop1_reg_dword(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_reg_double(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}

void lwc1(registers_t* regs, mem_t* mem, u32 instr) {
  u32 addr = (s64)(s16)instr + regs->gpr[base(instr)];
  u32 data = read32(mem, addr);
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FT(instr), data);
}

void swc1(registers_t* regs, mem_t* mem, u32 instr) {
  u32 addr = (s64)(s16)instr + regs->gpr[base(instr)];
  write32(mem, regs, addr, get_cop1_reg_word(&regs->cp1, &regs->cp0, FT(instr)));
}

void ldc1(registers_t* regs, mem_t* mem, u32 instr) {
  u32 addr = (s64)(s16)instr + regs->gpr[base(instr)];
  u64 data = read64(mem, addr);
  set_cop1_reg_dword(&regs->cp1, &regs->cp0, FT(instr), data);
}

void sdc1(registers_t* regs, mem_t* mem, u32 instr) {
  u32 addr = (s64)(s16)instr + regs->gpr[base(instr)];
  write64(mem, addr, get_cop1_reg_dword(&regs->cp1, &regs->cp0, FT(instr)));
}

void mfc1(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = (s32)get_cop1_reg_word(&regs->cp1, &regs->cp0, FS(instr));
}

void mtc1(registers_t* regs, u32 instr) {
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FS(instr), regs->gpr[RT(instr)]);
}