#include <disasm.h>
#include <core.h>
#include <log.h>

void init_disasm(disasm_t* disasm) {
  if(cs_open(CS_ARCH_MIPS, CS_MODE_MIPS64, &disasm->handle) != CS_ERR_OK) {
    logfatal("Could not initialize capstone\n");
  }
}

void destroy_disasm(disasm_t* disasm) {
  cs_close(&disasm->handle);
}