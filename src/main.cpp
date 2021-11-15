#include <gui.h>

int main(int argc, char* argv[]) {
  Gui gui("渋味");

  if(argc > 1) {
    gui.rom_file = argv[1];
    gui.Reset();
  }

  gui.MainLoop();
  return 0;
}