#pragma once

typedef struct mi_t mi_t;
typedef struct registers_t registers_t;

typedef enum {
  VI, SI, PI, AI, DP, SP
} interrupt_t;

void interrupt_raise(mi_t* mi, registers_t* regs, interrupt_t intr);
void interrupt_lower(mi_t* mi, registers_t* regs, interrupt_t intr);
void process_interrupt(mi_t* mi, registers_t* regs);
