#pragma once
#include <capstone.h>

struct Gui;

typedef struct core_t core_t;

struct Disasm {
	csh handle;
	cs_insn *insn;
	size_t count;
	Disasm();
	~Disasm();
  void Disassembly(Gui* gui, core_t* core);
};