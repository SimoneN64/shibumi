#include <emu.h>

int main(int argc, char* argv[]) {
  emu_t* emu = (emu_t*)calloc(1, sizeof(emu_t));
  init_emu(emu);

  if(argc > 1) {
    emu->core.running = load_rom(&emu->core.mem, argv[1]);
  }

  emu_run(emu);
  destroy_emu(emu);
  free(emu);

  return 0;
}
