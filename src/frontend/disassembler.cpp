#include <disassembler.hpp>
#include <util.hpp>

namespace Shibumi
{
Disassembler::Disassembler(core_t& core) {
  window = Gui::Window("Disassembler and debugger", [this, &core]() {
    Show(core);
  }, {ImGuiStyleVar_WindowRounding, 10.f});
  arrowDownID = CreateIDFromImage("resources/arrowDown.png");
  arrowRightID = CreateIDFromImage("resources/arrowRight.png");
}

void Disassembler::Show(core_t&) {
  const ImVec2 windowSize = ImGui::GetWindowSize();
  const ImVec2 fPCstrSize = ImGui::CalcTextSize("Follow Program Counter");
  ImGui::Text("Follow Program Counter");
  ImGui::SameLine(fPCstrSize.x + 15);
  ImGui::Checkbox("", &followPC);
  ImGui::SameLine(windowSize.x - 70);
  ImVec2 arrowSize{64, 64};
  ImGui::ImageButton(
    reinterpret_cast<ImTextureID>(
      reinterpret_cast<void*>(arrowDownID)
    ),
    arrowSize
  );
}
}