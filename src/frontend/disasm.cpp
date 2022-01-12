#include <disasm.hpp>
#include <log.h>
#include <imgui.h>
#include <frontend.hpp>

namespace Shibumi
{
static const ImVec4 colorsDisasm[4] = {
  ImVec4(1.0, 0.000, 0.0, 1.0),  // RED
  ImVec4(1.0, 0.619, 0.0, 1.0),  // ORANGE
  ImVec4(1.0, 0.988, 0.0, 1.0)
};

Disasm::Disasm() {
  if(cs_open(CS_ARCH_MIPS, CS_MODE_MIPS64, &handle) != CS_ERR_OK) {
    logfatal("Could not initialize capstone\n");
  }
}

Disasm::~Disasm() {
  cs_close(&handle);
}

void Disasm::Disassembly(Emulator& emu, core_t& core) {
  // emu.emuMutex.lock();

  u32 instructions[25] = {};
  u32 pc = core.cpu.regs.pc;
  u32 pointer = pc - (14 * 4);
  pointer -= (pointer & 3); // align the pointer

  if(emu.romLoaded) {
    for(int i = 0; i < 25; i++) {
      instructions[i] = read32(&core.mem, pointer + i * 4);
    }
  } else {
    memset(instructions, 0xFFFFFFFF, 100);
  }

  // emu.emuMutex.unlock();

  u8 code[100];
  memcpy(code, instructions, 100);
  
  count = cs_disasm(handle, code, sizeof(code), pointer, 25, &insn);
  ImVec2 window_size = ImGui::GetWindowSize();

  // emu.emuMutex.lock();

  if(ImGui::Button("Step", (ImVec2){ (window_size.x / 2) - 10, 20 })) {
    core.stepping = true;
    step(&core.cpu, &core.mem);
  }

  ImGui::SameLine(window_size.x / 2, 5);
  if(ImGui::Button("Run frame", (ImVec2){ (window_size.x / 2) - 10, 20 })) {
    core.stepping = true;
    for(int i = 0; i < 100000; i++) {
      step(&core.cpu, &core.mem);
    }
  }

  // emu.emuMutex.unlock();

  static int num_instr = 0;

  char run_n_instr_str[20];
  sprintf(run_n_instr_str, "Run %d instr", num_instr);
  run_n_instr_str[19] = '\0';

  // emu.emuMutex.lock();

  if(ImGui::Button(run_n_instr_str, (ImVec2){ (window_size.x / 3) - 10, 20 })) {
    core.stepping = true;
    for(int i = 0; i < num_instr; i++) {
      step(&core.cpu, &core.mem);
    }
  }
  
  // emu.emuMutex.unlock();

  ImGui::SameLine(window_size.x / 3, 5);
  ImGui::SetNextItemWidth(window_size.x / 10);
  ImGui::InputInt("Instruction count to run", &num_instr, 0, 0, ImGuiInputTextFlags_CharsNoBlank);
  
  static int addr = 0xFFFFFFFF;

  ImGui::InputInt("Address", &addr, 0, 0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank);

  ImGui::SameLine(window_size.x - (window_size.x / 4), 5);

  // emu.emuMutex.lock();

  if(ImGui::Button("Set breakpoint", (ImVec2){(window_size.x / 4) - 10, 20})) {
    core.break_addr = addr;
  }

  // emu.emuMutex.unlock();

  ImGui::Spacing();

  ImGui::BeginChild("frame");
  if(count > 0) {
    for(size_t j = 0; j < count; j++) {
      std::string op_str = insn[j].op_str;
      const float font_size = ImGui::GetFontSize() * op_str.length() / 2;
      switch(j) {
      case 12 ... 14:
        ImGui::TextColored(colorsDisasm[j & 3], "0x%" PRIx64 ":\t%s", insn[j].address, insn[j].mnemonic);
        ImGui::SameLine(window_size.x - font_size - 30, -1);
        ImGui::TextColored(colorsDisasm[j & 3], "%s", insn[j].op_str);
        break;
      default:
        ImGui::Text("0x%" PRIx64 ":\t%s", insn[j].address, insn[j].mnemonic);
        ImGui::SameLine(window_size.x - font_size - 30, -1);
        ImGui::Text("%s", insn[j].op_str);
      }
    }

    cs_free(insn, count);
  } else {
    ImGui::Text(emu.romLoaded ? "Could not disassemble instruction!" : "No game loaded!");
  }
  ImGui::EndChild();
}
}