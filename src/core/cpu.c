#include <cpu.h>
#include <instructions.h>
#include <utils.h>

void init_cpu(cpu_t *cpu) {
  init_registers(&cpu->regs);
}

void step(cpu_t *cpu, mem_t *mem) {
  registers_t* regs = &cpu->regs;
  regs->gpr[0] = 0;
  u32 instruction = read32(mem, regs->pc);
  regs->old_pc = regs->pc;
  regs->pc = regs->next_pc;
  regs->next_pc += 4;
  exec(regs, mem, instruction);
  handle_interrupt(cpu, mem);
}

void exec(registers_t* regs, mem_t* mem, u32 instr) {
  u8 mask = (instr >> 26) & 0x3F;
  switch(mask) { // TODO: named constants for clearer code
  case 0x00: special(regs, mem, instr); break;
  case 0x01: regimm(regs, mem, instr); break;
  case 0x02: j(regs, instr); break;
  case 0x03: jal(regs, instr); break;
  case 0x04: b(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
  case 0x05: b(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
  case 0x06: b(regs, instr, regs->gpr[RS(instr)] <= 0); break;
  case 0x08: addiu(regs, instr); break;
  case 0x09: addiu(regs, instr); break;
  case 0x0A: slti(regs, instr); break;
  case 0x0C: andi(regs, instr); break;
  case 0x0D: ori(regs, instr); break;
  case 0x0E: xori(regs, instr); break;
  case 0x0F: lui(regs, instr); break;
  case 0x10 ... 0x13: {
    u8 cop_number = (instr >> 26) & 3;
    u8 op_type = (instr >> 21) & 0x1F;
    switch(op_type) {
      case 0: mfcz(regs, instr, cop_number); break;
      case 2: cfcz(regs, instr, cop_number); break;
      case 4: mtcz(regs, instr, cop_number); break;
      case 6: ctcz(regs, instr, cop_number); break;
      default: logfatal("Unimplemented control registers transfer %02X (%08X) (%08X%08X)\n", 
                        (instr >> 21) & 0x1F, instr, (u32)(regs->pc >> 32), (u32)regs->pc);
    }
  } break;
  case 0x14: bl(regs, instr, regs->gpr[RS(instr)] == regs->gpr[RT(instr)]); break;
  case 0x15: bl(regs, instr, regs->gpr[RS(instr)] != regs->gpr[RT(instr)]); break;
  case 0x16: bl(regs, instr, regs->gpr[RS(instr)] <= 0); break;
  case 0x18: daddiu(regs, instr); break;
  case 0x21: lh(mem, regs, instr); break;
  case 0x23: lw(mem, regs, instr); break;
  case 0x24: lbu(mem, regs, instr); break;
  case 0x27: lwu(mem, regs, instr); break;
  case 0x28: sb(mem, regs, instr); break;
  case 0x29: sh(mem, regs, instr); break;
  case 0x2B: sw(mem, regs, instr); break;
  case 0x2F: break;
  default: logfatal("[CPU ERR] Unimplemented instruction %08X, PC: %08X%08X\n", bswap_32(instr), (u32)(regs->old_pc >> 32), (u32)regs->old_pc);
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
    case 0x0F: break;
    case 0x02: srl(regs, instr); break;
    case 0x04: sllv(regs, instr); break;
    case 0x06: srlv(regs, instr); break;
    case 0x08: jr(regs, instr); break;
    case 0x09: jalr(regs, instr); break;
    case 0x10: mfhi(regs, instr); break;
    case 0x12: mflo(regs, instr); break;
    case 0x19: multu(regs, instr); break;
    case 0x20: add(regs, instr); break;
    case 0x21: addu(regs, instr); break;
    case 0x23: subu(regs, instr); break;
    case 0x24: and_(regs, instr); break;
    case 0x25: or_(regs, instr); break;
    case 0x26: xor_(regs, instr); break;
    case 0x2A: slt(regs, instr); break;
    case 0x2B: sltu(regs, instr); break;
    default: logfatal("[CPU ERR] Unimplemented special instruction %08X, PC: %08X%08X\n", instr, (u32)(regs->old_pc >> 32), (u32)regs->old_pc);
  }
}

void regimm(registers_t* regs, mem_t *mem, u32 instr) {
  u8 mask = ((instr >> 16) & 0x1F);

  switch (mask) { // TODO: named constants for clearer code
  case 0x03: bl(regs, instr, regs->gpr[RS(instr)] >= 0); break;
  case 0x11:
    regs->gpr[31] = regs->pc + 4;
    b(regs, instr, regs->gpr[RS(instr)] >= 0);
    break;
  default:
    logfatal("[CPU ERR] Unimplemented regimm instruction %08X, PC: %08X%08X\n", instr, (u32)(regs->old_pc >> 32), (u32)regs->old_pc);
  }
}

static inline bool should_service_interrupt(registers_t* regs) {
  bool interrupts_pending = (regs->cp0.Status.im & regs->cp0.Cause.ip.raw) != 0;
  bool interrupts_enabled = regs->cp0.Status.ie == 1;
  bool currently_handling_exception = regs->cp0.Status.exl == 1;
  bool currently_handling_error = regs->cp0.Status.erl == 1;

  return interrupts_pending && interrupts_enabled &&
         !currently_handling_exception && !currently_handling_error;
}

void handle_interrupt(cpu_t* cpu, mem_t* mem) {
  if(should_service_interrupt(&cpu->regs)) {
    logfatal("Should service interrupt!\n");
  }
}