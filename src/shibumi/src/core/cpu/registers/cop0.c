#include <stdio.h>
#include <cop0.h>
#include <cpu.h>

void init_cop0(cop0_t* cop0) {
  cop0->Cause.raw = 0xB000007C;
  cop0->Random = 0x0000001F;
  cop0->Status.raw = 0x70400004;
  cop0->PRId = 0x00000B00;
  cop0->Config = 0x0006E463;
}

u32 get_cop0_reg_word(cop0_t* cop0, u8 index) {
  switch(index) {
    case 0: return cop0->Index;
    case 1: return cop0->Random;
    case 2: return cop0->EntryLo0;
    case 3: return cop0->EntryLo1;
    case 4: return cop0->Context;
    case 5: return cop0->PageMask;
    case 6: return cop0->Wired;
    case 7: return cop0->r7;
    case 8: return cop0->BadVaddr;
    case 9: return cop0->Count >> 1;
    case 10: return cop0->EntryHi;
    case 11: return cop0->Compare;
    case 12: return cop0->Status.raw;
    case 13: return cop0->Cause.raw;
    case 14: return cop0->EPC;
    case 15: return cop0->PRId;
    case 16: return cop0->Config;
    case 17: return cop0->LLAddr;
    case 18: return cop0->WatchLo;
    case 19: return cop0->WatchHi;
    case 20: return cop0->XContext;
    case 21: return cop0->r21;
    case 22: return cop0->r22;
    case 23: return cop0->r23;
    case 24: return cop0->r24;
    case 25: return cop0->r25;
    case 26: return cop0->ParityError;
    case 27: return cop0->CacheError;
    case 28: return cop0->TagLo;
    case 29: return cop0->TagHi;
    case 30: return cop0->ErrorEPC;
    case 31: return cop0->r31;
    default: return 0;
  }
}

void set_cop0_reg_word(cpu_t* cpu, mem_t* mem, u8 index, u32 value) {
  cop0_t* cop0 = &cpu->regs.cp0;
  switch(index) {
    case 0: cop0->Index = value; break;
    case 1: cop0->Random = value; break;
    case 2: cop0->EntryLo0 = value; break;
    case 3: cop0->EntryLo1 = value; break;
    case 4: cop0->Context = value; break;
    case 5: cop0->PageMask = value; break;
    case 6: cop0->Wired = value; break;
    case 7: cop0->r7 = value; break;
    case 8: cop0->BadVaddr = value; break;
    case 9: cop0->Count = (s64)value << 1; break;
    case 10: cop0->EntryHi = value; break;
    case 11: {
      cop0->Cause.ip.ip7 = 0;
      cop0->Compare = value;
    } break;
    case 12: {
      cop0->Status.raw &= ~STATUS_MASK;
      cop0->Status.raw |= value & STATUS_MASK;
    } break;
    case 13: {
      cop0_cause_t tmp;
      tmp.raw = value;
      cop0->Cause.ip.ip0 = tmp.ip.ip0;
      cop0->Cause.ip.ip1 = tmp.ip.ip1;
    } break;
    case 14: cop0->EPC = value; break;
    case 15: cop0->PRId = value; break;
    case 16: cop0->Config = value; break;
    case 17: cop0->LLAddr = value; break;
    case 18: cop0->WatchLo = value; break;
    case 19: cop0->WatchHi = value; break;
    case 20: cop0->XContext = value; break;
    case 21: cop0->r21 = value; break;
    case 22: cop0->r22 = value; break;
    case 23: cop0->r23 = value; break;
    case 24: cop0->r24 = value; break;
    case 25: cop0->r25 = value; break;
    case 26: cop0->ParityError = value; break;
    case 27: cop0->CacheError = value; break;
    case 28: cop0->TagLo = value; break;
    case 29: cop0->TagHi = value; break;
    case 30: cop0->ErrorEPC = value; break;
    case 31: cop0->r31 = value; break;
    default: break;
  }
}