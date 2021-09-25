#include <gui.h>
#include <stdio.h>
#include <log.h>

int main(int argc, const char* argv[]) {
  if(argc < 2) {
    logfatal("Please pass a valid Nintendo 64 rom as an argument.\nUsage: ./shibumi [rom]\n");
  }

  core_t core;
  gui_t gui;
  init_core(&core, argv[1]);
  init_gui(&gui);
  run(&gui, &core);
  destroy_disasm(&core.disasm);
  
  return 0;
}