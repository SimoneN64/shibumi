#pragma once
#include <capstone.h>
#include <core.h>

namespace Shibumi {
struct Emulator;
struct Disasm {
  csh handle;
	cs_insn *insn;
	size_t count;
	Disasm();
	~Disasm();
  void Disassembly(Emulator& emu, core_t& core);
};
}