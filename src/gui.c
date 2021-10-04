#include <gui.h>
#include <utils/log.h>

void init_gui(gui_t* gui) {
  gui->window = SDL_CreateWindow("Shibumi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
  gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_ACCELERATED);
  gui->texture = SDL_CreateTexture(gui->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 320, 240);
  gui->framebuffer = malloc(320 * 240 * 4);
}

void run(gui_t* gui, core_t* core) {
  bool running = true;
  u32 old_w = 0, old_h = 0;
  u8 old_format = 0;
  while(running) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type) {
      case SDL_QUIT: running = false; break;
    }

    run_core(core);
    update_framebuffer(gui, core, &old_w, &old_h, &old_format);
    
    SDL_RenderCopy(gui->renderer, gui->texture, NULL, NULL);
    SDL_RenderPresent(gui->renderer);
  }
}

void update_framebuffer(gui_t* gui, core_t* core, u32* old_w, u32* old_h, u8* old_format) {
  u32 w = core->mem.mmio.vi.width, h = 0.75 * w;
  u32 origin = core->mem.mmio.vi.origin & 0xFFFFFF;
  bool res_changed = (*old_w) != w || (*old_h) != h;
  bool format_changed = core->mem.mmio.vi.status.format != (*old_format);
  SDL_PixelFormatEnum format = SDL_PIXELFORMAT_RGBA8888;
  u8 depth = 4;

  if(res_changed) {
    (*old_w) = w;
    (*old_h) = h;
    SDL_RenderSetLogicalSize(gui->renderer, w, h);
  }

  if(format_changed) {
    (*old_format) = core->mem.mmio.vi.status.format;
    if((*old_format) == f5553) {
      format = SDL_PIXELFORMAT_RGBA5551; // Treat as 5551
      depth = 2;
      SDL_DestroyTexture(gui->texture);
      gui->texture = SDL_CreateTexture(gui->renderer, format, SDL_TEXTUREACCESS_STREAMING, w, h);
    }
  }
  
  gui->framebuffer = realloc(gui->framebuffer, w * h * depth);

  memcpy(gui->framebuffer, &core->mem.rdram[origin & RDRAM_DSIZE], w * h * depth);
  SDL_UpdateTexture(gui->texture, NULL, gui->framebuffer, w * depth);
}