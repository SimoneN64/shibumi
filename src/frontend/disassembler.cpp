#include <disassembler.hpp>
#include <util.hpp>

namespace Shibumi
{
Disassembler::Disassembler(core_t& core) {
  window = Gui::Window("Disassembler and debugger", [this, &core]() {
    Show(core);
  }, {ImGuiStyleVar_WindowRounding, 10.f});
  window.AddFlags(ImGuiWindowFlags_NoScrollbar);
}

void Disassembler::Show(core_t& core) {
  ImGui::Text("Follow Program Counter");
  ImGui::SameLine();
  ImGui::Checkbox("", &followPC);
  ImGui::SameLine();
  ImGui::Button("Continue");
  ImGui::SameLine();
  if(ImGui::Button("Step")) {
    step(&core.cpu, &core.mem);
  }
  ImGui::SameLine();
  if(ImGui::Button(core.running ? "Pause" : "Resume")) {
    core.running = !core.running;
  }
  ImGui::Text("Jump to");
  ImGui::SameLine();
  ImGui::PushItemWidth(80);
  ImGui::InputScalar("Address", ImGuiDataType_U32,
    &topAddress, nullptr, nullptr, nullptr,
    ImGuiInputTextFlags_CharsHexadecimal
  );
  ImGui::SameLine();
  ImGui::Button("Remove all breakpoints");
  ImGui::BeginChild("##disassembly");
  ImGui::EndChild();
}
}