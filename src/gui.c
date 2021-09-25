#include <gui.h>

void init_gui(gui_t* gui) {
  gui->framebuffer = malloc(320 * 240 * 4);
  gui->window = SDL_CreateWindow("Shibumi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
  gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_ACCELERATED);
  gui->texture = SDL_CreateTexture(gui->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 320, 240);
}

void run(gui_t* gui, core_t* core) {
  bool running = true;
  while(running) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type) {
      case SDL_QUIT: running = false; break;
    }

    run_core(core);

    gui->framebuffer = realloc(gui->framebuffer, core->mem.mmio.vi.width * (core->mem.mmio.vi.width * 3/4) * 4);
    memcpy(gui->framebuffer, &core->mem.rdram[core->mem.mmio.vi.origin & RDRAM_DSIZE], core->mem.mmio.vi.width * (core->mem.mmio.vi.width * 3/4) * 4);
    SDL_UpdateTexture(gui->texture, NULL, gui->framebuffer, core->mem.mmio.vi.width * 4);
    SDL_RenderCopy(gui->renderer, gui->texture, NULL, NULL);
    SDL_RenderPresent(gui->renderer);
  }
}