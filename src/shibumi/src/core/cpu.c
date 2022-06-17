#include <cpu.h>
#include <instruction/decode_instr.h>
#include <utils.h>

INLINE bool should_service_interrupt(registers_t* regs) {
  bool interrupts_pending = (regs->cp0.Status.im & regs->cp0.Cause.ip.raw) != 0;
  bool interrupts_enabled = regs->cp0.Status.ie == 1;
  bool currently_handling_exception = regs->cp0.Status.exl == 1;
  bool currently_handling_error = regs->cp0.Status.erl == 1;

  return interrupts_pending && interrupts_enabled &&
         !currently_handling_exception && !currently_handling_error;
}

INLINE void check_compare_interrupt(mi_t* mi, registers_t* regs) {
  regs->cp0.Count++;
  regs->cp0.Count &= 0x1FFFFFFFF;
  if(regs->cp0.Count == (u64)regs->cp0.Compare << 1) {
    regs->cp0.Cause.ip.ip7 = 1;
    update_interrupt(mi, regs);
  }
}

void fire_exception(registers_t* regs, exception_code_t code, int cop) {
  s64 pc = regs->pc;
  bool old_exl = regs->cp0.Status.exl;
  if(regs->in_delay_slot) {
    regs->cp0.Cause.branch_delay = true;
    pc -= 4;
  } else {
    regs->cp0.Cause.branch_delay = false;
  }

  if(!regs->cp0.Status.exl) {
    regs->cp0.Status.exl = true;
    regs->cp0.EPC = pc;
  }

  regs->cp0.Cause.cop_error = cop;
  regs->cp0.Cause.exc_code = code;

  if(regs->cp0.Status.bev) {
    logfatal("BEV bit set!\n");
  } else {
    switch(code) {
      case Int: case Mod:
      case AdEL: case AdES:
      case IBE: case DBE:
      case Sys: case Bp:
      case RI: case CpU:
      case Ov: case Tr:
      case FPE: case WATCH:
        logdebug("Exception fired! EPC = %016lX\n", regs->cp0.EPC);
        set_pc(regs, (s64)((s32)0x80000180));
        break;
      case TLBL: case TLBS:
        if(old_exl || regs->cp0.TlbError == INVALID) {
          set_pc(regs, (s64)((s32)0x80000180));
        } else {
          set_pc(regs, (s64)((s32)0x80000000));
        }
        break;
      default: logfatal("Unhandled exception! %d\n", code);
    }
  }
}

INLINE void handle_interrupt(cpu_t* cpu, mem_t* mem) {
  if(should_service_interrupt(&cpu->regs)) {
    fire_exception(&cpu->regs, Int, 0);
  }
}

void init_cpu(cpu_t *cpu) {
  init_registers(&cpu->regs);
}

void step(cpu_t *cpu, mem_t *mem) {
  registers_t* regs = &cpu->regs;
  check_compare_interrupt(&mem->mmio.mi, regs);
  handle_interrupt(cpu, mem);
  regs->gpr[0] = 0;
  u32 instruction = read32(mem, regs, regs->pc);
  // logdebug("PC: %016lX\n", regs->pc);
  regs->old_pc = regs->pc;
  regs->pc = regs->next_pc;
  regs->next_pc += 4;
  regs->in_delay_slot = false;
  exec(cpu, mem, instruction);
}