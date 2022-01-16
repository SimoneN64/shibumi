#pragma once
#include <set>
#include <core.h>
#include <gui.hpp>

namespace Shibumi
{
struct Disassembler {
  Disassembler(core_t&);
  void Show(core_t&);
  Gui::Window window;
private:
  u32 topAddress = 0;
  bool followPC = false;
  std::set<u32> breakpoints;
};
}