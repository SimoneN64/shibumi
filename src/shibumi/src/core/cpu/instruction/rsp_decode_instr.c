#include <rsp_decode_instr.h>
#include <log.h>

void rsp_exec(rsp_t* rsp, u32 instr) {
  switch(instr) {
    case 0: break;
    default: logfatal("Unhandled RSP instruction %08X\n", instr);
  }
}