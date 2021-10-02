#include <gui.h>
#include <stdio.h>
#include <log.h>

core_t core;

void dump_rdram();

int main(int argc, const char* argv[]) {
  if(argc < 2) {
    logfatal("Please pass a valid Nintendo 64 rom as an argument.\nUsage: ./shibumi [rom]\n");
  }

  atexit(dump_rdram);

  gui_t gui;
  init_core(&core, argv[1]);
  init_gui(&gui);
  run(&gui, &core);
  destroy_disasm(&core.disasm);
  
  return 0;
}

void dump_rdram() {
  FILE* f = fopen("rdram.dump", "wb");
  fwrite(core.mem.rdram, 1, RDRAM_SIZE, f);
  fclose(f);
}