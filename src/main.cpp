#include <gui.h>
#include <stdio.h>
#include <log.h>

int main(int argc, char* argv[]) {
  Gui gui("渋味");

  if(argc > 1) {
    gui.rom_file = argv[1];
    gui.reset();
  }

  gui.main_loop();
  return 0;
}