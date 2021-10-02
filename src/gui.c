#include <gui.h>

void init_gui(gui_t* gui) {
  gui->window = SDL_CreateWindow("Shibumi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
  gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(gui->renderer, 320, 240);
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

    u32 origin = core->mem.mmio.vi.origin & 0xFFFFFF;
    
    memcpy(gui->framebuffer, &core->mem.rdram[origin & RDRAM_DSIZE], 320 * 240 * 4);
    SDL_UpdateTexture(gui->texture, NULL, gui->framebuffer, 320 * 4);
    SDL_RenderCopy(gui->renderer, gui->texture, NULL, NULL);
    SDL_RenderPresent(gui->renderer);
  }
}