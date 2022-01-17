#include <fpu_instructions.h>
#define FD(x) (((x) >> 6) & 0x1F)
#define FS(x) (((x) >> 11) & 0x1F)

void cvtdw(registers_t* regs, u32 instr) {
  set_cop1_register_double(
    &regs->cp1,
    &regs->cp0,
    FD(instr),
    get_cop1_register_double(
      &regs->cp1,
      &regs->cp0,
      FS(instr)
    )
  );
}