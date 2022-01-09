#include <frontend.hpp>
#include <fmt/core.h>

namespace Shibumi
{
Disasm::Disasm() {

}

Disasm::~Disasm() {

}

void Disasm::Disassembly(Emulator& emu, core_t& core) {

}

Logger::Logger() {

}

void Logger::LogWindow(Emulator& emu) {

}

Emulator::Emulator(int w, int h, const std::string& title) 
    : window(w, h, title), 
      image(context.id, context.old_w, context.old_h, true),
      screen("Screen") {
  NFD_Init();
  init_core(&core);

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

  window.AddEvent([this]() {
    run_frame(&core);
  });

  window.AddEvent([this]() {
    context.UpdateTexture(core);
  });

  screen.AddChild(&image);
  window.AddWidget(&screen);
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