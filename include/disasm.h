#pragma once
#include <capstone/capstone.h>

typedef struct core_t core_t;

typedef struct {
	csh handle;
	cs_insn *insn;
	size_t count;
} disasm_t;

void init_disasm(disasm_t* disasm);
char* disasm_run(core_t* core, disasm_t* disasm);