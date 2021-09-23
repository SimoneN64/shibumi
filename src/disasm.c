#include <disasm.h>
#include <core.h>
#include <log.h>

void init_disasm(disasm_t* disasm) {
  if(cs_open(CS_ARCH_MIPS, CS_MODE_MIPS64 | CS_MODE_BIG_ENDIAN, &disasm->handle) != CS_ERR_OK) {
    logfatal("Could not initialize capstone\n");
  }
}

char* disasm_run(core_t* core, disasm_t* disasm) {
  u32 code = read32(&core->mem, core->cpu.regs.pc);
  disasm->count = cs_disasm(disasm->handle, (u8*)&code, sizeof(code), core->cpu.regs.pc, 1, &disasm->insn);
  char* result = malloc(1000);
  if(disasm->count > 0) {
    for(int i = 0; i < disasm->count; i++) {
      sprintf(result, "0x%"PRIx64":\t%s\t\t%s\n", disasm->insn[i].address, disasm->insn[i].mnemonic, disasm->insn[i].op_str);
    }

    cs_free(disasm->insn, disasm->count);
  } else {
    logfatal("Unable to disassemble instruction %08X\n", code);
  }

  cs_close(&disasm->handle);
  return result;
}