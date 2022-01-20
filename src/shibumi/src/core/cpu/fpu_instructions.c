#include <fpu_instructions.h>
#include <log.h>
#include <math.h>
#include <fenv.h>

#define push_rounding_mode(x) \
  const int original_rounding_mode = fegetround(); \
  fesetround(x)

#define pop_rounding_mode \
  fesetround(original_rounding_mode)

INLINE bool get_cond_double(double fs, double ft, comp_conds cond) {
  switch(cond) {
    case T: case GLE: return true;
    case EQ: return fs == ft;
    case NGT: case NGL: case NGLE: case LE:
      return fs <= ft;
    case NGE: case LT: return fs < ft;
    case NEQ: return fs != ft;
    case GL: return fs > ft || fs < ft;
    case NLT: case GE: return fs >= ft;
    case NLE: case GT: return fs > ft;
    default: logfatal("Unhandled compare condition %d\n", cond);
  }
}

INLINE bool get_cond_float(float fs, float ft, comp_conds cond) {
  switch(cond) {
    case T: case GLE: return true;
    case EQ: return fs == ft;
    case NGT: case NGL: case NGLE: case LE:
      return fs <= ft;
    case NGE: case LT: return fs < ft;
    case NEQ: return fs != ft;
    case GL: return fs > ft || fs < ft;
    case NLT: case GE: return fs >= ft;
    case NLE: case GT: return fs > ft;
    default: logfatal("Unhandled compare condition %d\n", cond);
  }
}

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

void adds(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  float ft = get_cop1_reg_float(&regs->cp1, &regs->cp0, FT(instr));
  float result = fs + ft;
  set_cop1_reg_float(&regs->cp1, &regs->cp0, FD(instr), result);
}

void addd(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  double ft = get_cop1_reg_double(&regs->cp1, &regs->cp0, FT(instr));
  double result = fs + ft;
  set_cop1_reg_double(&regs->cp1, &regs->cp0, FD(instr), result);
}

void ceills(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  s64 result = ceilf(fs);
  set_cop1_reg_dword(&regs->cp1, &regs->cp0, FD(instr), result);
}

void ceilws(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  s32 result = ceilf(fs);
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FD(instr), result);
}

void ceilld(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  s64 result = ceil(fs);
  set_cop1_reg_dword(&regs->cp1, &regs->cp0, FD(instr), result);
}

void ceilwd(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  s32 result = ceil(fs);
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FD(instr), result);
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

void ccondd(registers_t* regs, u32 instr, comp_conds cond) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  double ft = get_cop1_reg_double(&regs->cp1, &regs->cp0, FT(instr));
  regs->cp1.fcr31.compare = get_cond_double(fs, ft, cond);
}

void cconds(registers_t* regs, u32 instr, comp_conds cond) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  float ft = get_cop1_reg_float(&regs->cp1, &regs->cp0, FT(instr));
  regs->cp1.fcr31.compare = get_cond_float(fs, ft, cond);
}

void divs(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  float ft = get_cop1_reg_float(&regs->cp1, &regs->cp0, FT(instr));
  set_cop1_reg_float(&regs->cp1, &regs->cp0, FD(instr), fs / ft);
}

void divd(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  double ft = get_cop1_reg_double(&regs->cp1, &regs->cp0, FT(instr));
  set_cop1_reg_double(&regs->cp1, &regs->cp0, FD(instr), fs / ft);
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

void truncws(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  s32 result = (s32)truncf(fs);
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FD(instr), result);
}

void truncwd(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  s32 result = (s32)trunc(fs);
  set_cop1_reg_word(&regs->cp1, &regs->cp0, FD(instr), result);
}

void truncls(registers_t* regs, u32 instr) {
  float fs = get_cop1_reg_float(&regs->cp1, &regs->cp0, FS(instr));
  s64 result = (s64)truncf(fs);
  set_cop1_reg_dword(&regs->cp1, &regs->cp0, FD(instr), result);
}

void truncld(registers_t* regs, u32 instr) {
  double fs = get_cop1_reg_double(&regs->cp1, &regs->cp0, FS(instr));
  s64 result = (s64)trunc(fs);
  set_cop1_reg_dword(&regs->cp1, &regs->cp0, FD(instr), result);
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