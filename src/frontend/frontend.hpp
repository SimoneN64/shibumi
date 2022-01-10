#pragma once
#include <context.hpp>
#include <imgui_logger.h>
#include <atomic>
#include <nfd.hpp>
#include <log.h>
#include <disasm.hpp>

namespace Shibumi
{
using namespace ::Gui;

static const std::string messageTypeStrings[3] = {"[INFO]", "[WARNING]", "[FATAL]"};

struct Logger {
  Logger();
  void LogWindow(Emulator& emu);
  ImGui::Logger logger;
  std::string oldMessage = "NULL";
  message_type oldMessageType = INFO;
  bool shown = true;
};

struct Emulator {
  Emulator(int w, int h, const std::string& title);
  void Run();
  void OpenFile();
  void Reset();
  void Stop();
  void Start();
  bool romLoaded = false, cacheAboutWindow = false;
  std::atomic_bool emuQuit = false;
  nfdchar_t* romFile;
  Context context;
  Logger logger;
  Disasm disasm;
  core_t core;

  HostWindow window;
    Window screen{"Screen"};
      ImagePainter image;
    Window disassembler;
    MenuBar mainMenuBar;
      Menu file;
        MenuItem open, exit;
      Menu emulation;
        MenuItem pause, reset, stop;
      Menu help;
        MenuItem about;
  
  void AddEvents();
  void AddKeyHandlers();
  void CreateWindows();
  void CreateMainMenuBar();
  void CreateFileMenu();
  void CreateEmulationMenu();
  void CreateHelpMenu();
};
}