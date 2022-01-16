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
  std::set<u32> breakpoints;
};
}