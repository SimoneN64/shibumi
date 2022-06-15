#include <cop0_instructions.h>
#include <cpu.h>
#include <log.h>

void mtc0(cpu_t* cpu, mem_t* mem, u32 instr) {
  set_cop0_reg_word(cpu, mem, RD(instr), cpu->regs.gpr[RT(instr)]);
}

void mfc0(registers_t* regs, u32 instr) {
  regs->gpr[RT(instr)] = (s32)get_cop0_reg_word(&regs->cp0, RD(instr));
}

void eret(registers_t* regs) {
  if(regs->cp0.Status.erl) {
    set_pc(regs, (s64)regs->cp0.ErrorEPC);
    regs->cp0.Status.erl = false;
  } else {
    set_pc(regs, (s64)regs->cp0.EPC);
    regs->cp0.Status.exl = false;
  }
}

void tlbr(registers_t* regs) {
  int index = regs->cp0.Index & 0b111111;
  if (index >= 32) {
    logfatal("TLBR with TLB index %d", index);
  }

  tlb_entry_t entry = regs->cp0.tlb[index];

  regs->cp0.EntryHi.raw = entry.EntryHi.raw;
  regs->cp0.EntryLo0.raw = entry.EntryLo0.raw & 0x3FFFFFFF;
  regs->cp0.EntryLo1.raw = entry.EntryLo1.raw & 0x3FFFFFFF;

  regs->cp0.EntryLo0.g = entry.global;
  regs->cp0.EntryLo1.g = entry.global;
  regs->cp0.PageMask.raw = entry.PageMask.raw;
}

void tlbwi(registers_t* regs) {
  page_mask_t page_mask;
  page_mask = regs->cp0.PageMask;
  u32 top = page_mask.mask & 0xAAA;
  page_mask.mask = top | (top >> 1);

  int index = regs->cp0.Index & 0x3F;
  if(index >= 32) {
    logfatal("TLBWI with TLB index %d\n", index);
  }

  regs->cp0.tlb[index].EntryHi.raw  = regs->cp0.EntryHi.raw;
  regs->cp0.tlb[index].EntryHi.vpn2 &= ~page_mask.mask;

  regs->cp0.tlb[index].EntryLo0.raw = regs->cp0.EntryLo0.raw & 0x03FFFFFE;
  regs->cp0.tlb[index].EntryLo1.raw = regs->cp0.EntryLo1.raw & 0x03FFFFFE;
  regs->cp0.tlb[index].PageMask.raw = page_mask.raw;

  regs->cp0.tlb[index].global = regs->cp0.EntryLo0.g && regs->cp0.EntryLo1.g;
}

void tlbp(registers_t* regs) {
  int match = -1;
  tlb_entry_t* entry = tlb_try_match(regs, regs->cp0.EntryHi.raw, &match);
  if(entry && match >= 0) {
    regs->cp0.Index = match;
  } else {
    regs->cp0.Index = 0x80000000;
  }
}
