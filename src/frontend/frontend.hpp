#pragma once
#include <context.hpp>
#include <imgui_logger.h>
#include <atomic>
#include <nfd.hpp>
#include <log.h>
#include <thread>
#include <mutex>

namespace Shibumi
{
using namespace ::Gui;

static const std::string messageTypeStrings[3] = {"[INFO]", "[WARNING]", "[FATAL]"};

struct Logger {
  Logger(Emulator& emu);
  void LogWindow(Emulator& emu);
  Window logWindow;
  ImGui::Logger logger;
  std::string oldMessage = "NULL";
  message_type oldMessageType = INFO;
  bool shown = true;
};

struct Emulator {
  ~Emulator() { Stop(); }
  Emulator(int w, int h, const std::string& title);
  void Run();
  void OpenFile();
  void Reset();
  void Stop();
  void Start();
  bool romLoaded = false, cacheAboutWindow = false;
  std::atomic_bool emuQuit = false;
  std::thread emuThread;
  std::mutex emuMutex;
  bool previouslyStopped = false;
  nfdchar_t* romFile;
  core_t core;

  HostWindow window;
    Window screen{"Screen", nullptr, {ImGuiStyleVar_WindowRounding, 10.f}};
      ImagePainter image;
    MenuBar mainMenuBar;
      Menu file;
        MenuItem open, exit;
      Menu emulation;
        MenuItem pause, reset, stop;
      Menu help;
        MenuItem about;
  
  Logger logger;
  Context context;
  
  double frametime = 0;
  void AddEvents();
  void AddKeyHandlers();
  void CreateWindows();
  void CreateMainMenuBar();
  // void CreateFileMenu();
  // void CreateEmulationMenu();
  // void CreateHelpMenu();
  void CreateHelpWindow();
};
}