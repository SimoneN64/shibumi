#pragma once
#include <context.hpp>
#include <imgui_logger.h>
#include <capstone.h>
#include <atomic>
#include <nfd.hpp>

namespace Shibumi
{
using namespace ::Gui;
struct Emulator;
struct Disasm {
  csh handle;
	cs_insn *insn;
	size_t count;
	Disasm();
	~Disasm();
  void Disassembly(Emulator& emu, core_t& core);
};

static const std::string message_type_strings[3] = {"[INFO]", "[WARNING]", "[FATAL]"};

struct Logger {
  Logger();
  void LogWindow(Emulator& emu);
  ImGui::Logger logger;
};


struct Emulator {
  Emulator(int w, int h, const std::string& title);
  void Run();
  void OpenFile();
  void Reset();
  void Stop();
  void Start();
  bool romLoaded = false;
  std::atomic_bool emuQuit = false;
  nfdchar_t* romFile;
  Context context;
  Logger logger;
  Disasm disasm;
  core_t core;
  HostWindow window;
  Window screen;
  ImagePainter image;
};
}