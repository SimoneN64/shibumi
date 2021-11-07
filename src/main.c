#include <gui.h>
#include <stdio.h>
#include <log.h>

int main(int argc, char* argv[]) {
  gui_t gui;
  init_gui(&gui, "渋味");

  if(argc > 1) {
    gui.rom_file = argv[1];
    reset(&gui);
  }

  main_loop(&gui);
  
  destroy_gui(&gui);
  return 0;
}