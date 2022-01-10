#include <frontend.hpp>
#include <fmt/core.h>

namespace Shibumi
{
Logger::Logger() {
  logger.InfoStr = messageTypeStrings[0].c_str();
  logger.WarnStr = messageTypeStrings[1].c_str();
  logger.ErrorStr = messageTypeStrings[2].c_str();
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
    : window(w, h, title), 
      image(context.id, context.old_w, context.old_h, true) {
  NFD_Init();
  init_core(&core);

  CreateMainMenuBar();
  CreateWindows();
  AddKeyHandlers();
  AddEvents();
}

void Emulator::CreateMainMenuBar() {
  mainMenuBar = MenuBar(nullptr, true); 
  
  CreateFileMenu();
  CreateEmulationMenu();
  CreateHelpMenu();

  mainMenuBar.AddChild(&file);
  mainMenuBar.AddChild(&emulation);
  mainMenuBar.AddChild(&help);
}

void Emulator::CreateFileMenu() {
  file = Menu("File", nullptr);
  open = MenuItem("Open", [this]() {
    OpenFile();
  });

  exit = MenuItem("Exit", [this]() {
    window.Close();
  });

  file.AddChild(&open);
  file.AddChild(&exit);
}

void Emulator::CreateEmulationMenu() {
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
}

void Emulator::CreateHelpMenu() {
  help = Menu("Help", nullptr);
  about = MenuItem("About", [this]() {
    cacheAboutWindow = true;
  });

  help.AddChild(&about);

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
      ImGui::Begin("About shibumi", &cacheAboutWindow, ImGuiWindowFlags_NoResize);
      ImGui::SetCursorPos({aboutWindowSize.x / 16, aboutWindowSize.y / 7});
      ImGui::Text(aboutText);
      ImGui::End();
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
    window.Close();
  });
}

void Emulator::AddEvents() {
  window.AddEvent([this]() {
    run_frame(&core);
  });

  window.AddEvent([this]() {
    logger.LogWindow(*this);
  });

  window.AddEvent([this]() {
    context.UpdateTexture(core);
  });
}

void Emulator::CreateWindows() {
  disassembler = Window("Disassembly", [this]() {
    disasm.Disassembly(*this, core);
  });

  screen.AddChild(&image);
  window.AddWidget(&screen);
  window.AddWidget(&disassembler);
  window.AddWidget(&mainMenuBar);
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
  emuQuit = true;
  init_core(&core);
  romLoaded = false;
  core.running = false;
}

void Emulator::Start() {
  romLoaded = load_rom(&core.mem, romFile);
  emuQuit = !romLoaded;
  core.running = romLoaded;
  //if(romLoaded) {
  //  pthread_create(&emu_thread, NULL, core_callback, (void*)this);
  //}
}
}