#include <gui.h>
#include <stdio.h>
#include <log.h>

int main() {
  gui_t gui;
  init_gui(&gui, "渋味");

  main_loop(&gui);
  
  destroy_gui(&gui);
  return 0;
}