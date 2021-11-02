#pragma once
#include <capstone.h>

typedef struct {
	csh handle;
	cs_insn *insn;
	size_t count;
} disasm_t;

void init_disasm(disasm_t* disasm);
void destroy_disasm(disasm_t* disasm);