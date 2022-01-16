#include <disassembler.hpp>

namespace Shibumi
{
Disassembler::Disassembler(core_t& core) {
  window = Gui::Window("Disassembler and debugger", [this, &core]() {
    Show(core);
  }, {ImGuiStyleVar_WindowRounding, 10.f});
}

void Disassembler::Show(core_t&) {

}
}