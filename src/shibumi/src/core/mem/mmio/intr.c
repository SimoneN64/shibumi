#include <intr.h>
#include <registers.h>
#include <mi.h>
#include <log.h>

void update_interrupt(mi_t* mi, registers_t* regs) {
  bool interrupt = mi->mi_intr.raw & mi->mi_intr_mask.raw;
  regs->cp0.Cause.ip.ip2 = interrupt;
}

void interrupt_raise(mi_t* mi, registers_t* regs, interrupt_t intr) {
  switch(intr) {
    case VI:
      //logdebug("VI interrupt raised!\n");
      mi->mi_intr.vi = true;
      break;
    case SI:
      //logdebug("SI interrupt raised!\n");
      mi->mi_intr.si = true;
      break;
    case PI:
      //logdebug("PI interrupt raised!\n");
      mi->mi_intr.pi = true;
      break;
    case AI:
      //logdebug("AI interrupt raised!\n");
      mi->mi_intr.ai = true;
      break;
    case DP:
      //logdebug("DP interrupt raised!\n");
      mi->mi_intr.dp = true;
      break;
    case SP:
      //logdebug("SP interrupt raised!\n");
      mi->mi_intr.sp = true;
      break;
  }

  update_interrupt(mi, regs);
}

void interrupt_lower(mi_t* mi, registers_t* regs, interrupt_t intr) {
  switch(intr) {
    case VI:
      //logdebug("VI interrupt raised!\n");
      mi->mi_intr.vi = false;
      break;
    case SI:
      //logdebug("SI interrupt lowered!\n");
      mi->mi_intr.si = false;
      break;
    case PI:
      //logdebug("PI interrupt lowered!\n");
      mi->mi_intr.pi = false;
      break;
    case AI:
      //logdebug("AI interrupt lowered!\n");
      mi->mi_intr.ai = false;
      break;
    case DP:
      //logdebug("DP interrupt lowered!\n");
      mi->mi_intr.dp = false;
      break;
    case SP:
      //logdebug("SP interrupt lowered!\n");
      mi->mi_intr.sp = false;
      break;
  }

  update_interrupt(mi, regs);
}
