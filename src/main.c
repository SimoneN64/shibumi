#include <emu.h>

int main(int argc, char* argv[]) {
  emu_t emu;
  init_emu(&emu);
  if(argc > 1) {
    emu.core.running = load_rom(&emu.core.mem, argv[1]);
  }

  emu_run(&emu);

  return 0;
}