#include <frontend.hpp>

int main(int argc, char* argv[]) {
  Shibumi::Emulator emu(800, 600, "shibumi");
  emu.Run();
  return 0;
}