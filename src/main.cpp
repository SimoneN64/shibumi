#include <frontend.hpp>

int main(int argc, char* argv[]) {
  Shibumi::Emulator emu(1280, 720, "shibumi");
  emu.Run();
  return 0;
}