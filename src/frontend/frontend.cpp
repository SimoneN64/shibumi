#include <frontend.hpp>
#include <fmt/core.h>

namespace Shibumi
{
Logger::Logger(Emulator& emu) {
  logger.InfoStr = messageTypeStrings[0].c_str();
  logger.WarnStr = messageTypeStrings[1].c_str();
  logger.ErrorStr = messageTypeStrings[2].c_str();
  logWindow = Window("Logs", [this, &emu]() {
    LogWindow(emu);
  }, {ImGuiStyleVar_WindowRounding, 10.f});
  emu.window.AddWidget(&logWindow);
}

void Logger::LogWindow(Emulator& emu) {
  std::string finalMessage{};
  if(last_message != nullptr && strcmp(last_message, "") && strcmp(last_message, oldMessage.c_str())) {
    if(last_message_type == FATAL) {
      emu.Stop();
    }

    oldMessage = std::string(last_message);
    oldMessageType = last_message_type;
    finalMessage = messageTypeStrings[last_message_type] + " " + oldMessage;
    logger.AddLog("%s", finalMessage.c_str());
  }
  logger.Draw("Logs", &shown);
}

Emulator::Emulator(int w, int h, const std::string& title) 
    : window(w, h, title), context(), logger(*this), disassembler(core) {
  NFD_Init();
  init_core(&core);
  AddKeyHandlers();
  AddEvents();
  CreateMainMenuBar();
  CreateWindows();

  image = ImagePainter(context.id, context.old_w, context.old_h, true);

  /* emuThread = std::thread([this]() {
    while(!emuQuit) {
      auto start = SteadyClock::now();
      run_frame(&core);
      frametime = Milliseconds(SteadyClock::now() - start).count();
    }
  }); */
}

void Emulator::CreateMainMenuBar() {
  /*mainMenuBar = MenuBar(nullptr, true); 
  
  CreateFileMenu();
  CreateEmulationMenu();
  CreateHelpMenu();

  mainMenuBar.AddChild(&file);
  mainMenuBar.AddChild(&emulation);
  mainMenuBar.AddChild(&help);
  window.AddWidget(&mainMenuBar);*/
  window.AddEvent([this]() {
    if(ImGui::BeginMainMenuBar()) {
      if(ImGui::BeginMenu("File")) {
        if(ImGui::MenuItem("Open")) {
          OpenFile();
        }
        if(ImGui::MenuItem("Exit")) {
          Stop();
          window.Close();
        }
        ImGui::EndMenu();
      }
      if(ImGui::BeginMenu("Emulation")) {
        if(ImGui::MenuItem(core.running ? "Pause" : "Resume")) {
          if(romLoaded) {
            core.running = !core.running;
          }
        }
        if(ImGui::MenuItem("Reset")) {
          Reset();
        }
        if(ImGui::MenuItem("Stop")) {
          Stop();
        }
        ImGui::EndMenu();
      }
      if(ImGui::BeginMenu("Help")) {
        if(ImGui::MenuItem("About")) {
          cacheAboutWindow = true;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  });

  CreateHelpWindow();
}

/* void Emulator::CreateFileMenu() {
  file = Menu("File", nullptr);
  open = MenuItem("Open", [this]() {
    OpenFile();
  });

  exit = MenuItem("Exit", [this]() {
    Stop();
    window.Close();
  });

  file.AddChild(&open);
  file.AddChild(&exit);
} */

/* void Emulator::CreateEmulationMenu() {
  emulation = Menu("Emulation", nullptr);
  pause = MenuItem("Pause", [this]() {
    if(romLoaded) {
      core.running = !core.running;
    }
  });

  reset = MenuItem("Reset", [this]() {
    Reset();
  });

  stop = MenuItem("Stop", [this]() {
    Stop();
  });

  emulation.AddChild(&pause);
  emulation.AddChild(&reset);
  emulation.AddChild(&stop);
} */

//void Emulator::CreateHelpMenu() {
void Emulator::CreateHelpWindow() {
  // help = Menu("Help", nullptr);
  // about = MenuItem("About", [this]() {
  //   cacheAboutWindow = true;
  // });

  // help.AddChild(&about);

  window.AddEvent([this]() {
    if(cacheAboutWindow) {
      const auto aboutText = R"(
      shibumi is a Nintendo 64 emulator that strives to provide a wonderful
              experience to both casual players and speedrunners.
        It comes with a variety of tools such as TAS recording and input,
          frame advancing, rewinding, but also debugging tools such as
            registers watching, disassembly, instruction stepping
                              and breakpoints.
    
          Nintendo 64 is a registered trademark of Nintendo Co. Ltd.
      )";
      const auto aboutTextSize = ImGui::CalcTextSize(aboutText);
      const ImVec2 aboutWindowSize = {
        static_cast<float>(aboutTextSize.x * 1.25),
        static_cast<float>(aboutTextSize.y * 1.25)
      };
      ImGui::SetNextWindowSize(aboutWindowSize);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.f);
      ImGui::Begin("About shibumi", &cacheAboutWindow, ImGuiWindowFlags_NoResize);
      ImGui::SetCursorPos({aboutWindowSize.x / 16, aboutWindowSize.y / 7});
      ImGui::Text(aboutText);
      ImGui::End();
      ImGui::PopStyleVar();
    }
  });
}

void Emulator::AddKeyHandlers() {
  window.AddKeyHandler(Key::O, [this](){
    OpenFile();
  });

  window.AddKeyHandler(Key::P, [this](){
    if(romLoaded) {
      core.running = !core.running;
    }
  });

  window.AddKeyHandler(Key::Escape, [this](){
    Stop();
    window.Close();
  });
}

void Emulator::AddEvents() {
  window.AddEvent([this] () {
    std::string title = fmt::format("shibumi [{:.2f} fps] [{:.2f} ms]", 1000 / frametime, frametime);
    window.SetWindowTitle(title);
  });

  window.AddEvent([this]() {
    context.UpdateTexture(*this, core);
  });

  window.SetEventOnClose([this]() {
    Stop();
  });
}

void Emulator::CreateWindows() {
  screen.AddChild(&image);
  window.AddWidget(&screen);
  window.AddWidget(&disassembler.window);
}

void Emulator::Run() {
  window.Run();
}

void Emulator::OpenFile() {
  nfdfilteritem_t filter = { "Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64" };
  nfdresult_t result = NFD_OpenDialog(&romFile, &filter, 1, ".");
  if(result == NFD_OKAY) {
    Reset();
  }
}

void Emulator::Reset() {
  Stop();
  Start();
}

void Emulator::Stop() {
  if(previouslyStopped) return;
  previouslyStopped = true;
  emuQuit = true;
  // emuThread.join();
  romLoaded = false;
  core.running = false;
  init_core(&core);
}

void Emulator::Start() {
  previouslyStopped = false;
  romLoaded = load_rom(&core.mem, romFile);
  emuQuit = !romLoaded;
  core.running = romLoaded;
  if(romLoaded) {
    /* emuThread = std::thread([this]() {
      while(!emuQuit) {
        auto start = SteadyClock::now();
        run_frame(&core);
        frametime = Milliseconds(SteadyClock::now() - start).count();
      }
    }); */
  }
}
}