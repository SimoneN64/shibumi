#include <emu.h>
#include <mem.h>

void destroy_emu(emu_t* emu) {
  free(emu->framebuffer);
  SDL_DestroyTexture(emu->texture);
  SDL_DestroyRenderer(emu->renderer);
  SDL_DestroyWindow(emu->window);
  SDL_Quit();
}

void init_emu(emu_t* emu) {
  init_core(&emu->core);
  emu->currentW = 320;
  emu->currentH = 240;
  emu->sdlFormat = SDL_PIXELFORMAT_RGBA32;
  emu->currentFormat = f8888;

  SDL_Init(SDL_INIT_VIDEO);
  emu->window = SDL_CreateWindow(
    "shibumi",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    W,
    H,
    SDL_WINDOW_RESIZABLE
  );
  emu->renderer = SDL_CreateRenderer(emu->window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(emu->renderer, (int)emu->currentW, (int)emu->currentH);
  emu->texture = SDL_CreateTexture(
    emu->renderer,
    emu->sdlFormat,
    SDL_TEXTUREACCESS_STREAMING,
    (int)emu->currentW,
    (int)emu->currentH
 );
  emu->framebuffer = (u8*)calloc(emu->currentW * emu->currentH, 4);
  NFD_Init();
}

void emu_present(emu_t* emu) {
  mem_t* mem = &emu->core.mem;
  const u32 w = mem->mmio.vi.width, h = 0.75 * w;
  const u32 origin = mem->mmio.vi.origin & 0xFFFFFF;
  const u8 format = mem->mmio.vi.status.format;
  u8 depth = format == f8888 ? 4 : format == f5553 ? 2 : 0;
  bool reconstructTexture = false;
  const bool resChanged = emu->currentW != w || emu->currentH != h;
  const bool formatChanged = emu->currentFormat != format;
  if(resChanged) {
    emu->currentW = w;
    emu->currentH = h;

    reconstructTexture = true;
  }

  if(formatChanged) {
    emu->currentFormat = format;
    if(format == f5553) {
      emu->sdlFormat = SDL_PIXELFORMAT_RGBA5551;
      depth = 2;
    } else if(format == f8888) {
      emu->sdlFormat = SDL_PIXELFORMAT_RGBA32;
      depth = 4;
    }

    reconstructTexture = true;
  }

  if(reconstructTexture) {
    SDL_DestroyTexture(emu->texture);
    free(emu->framebuffer);
    emu->framebuffer = (u8*)calloc(w * h, depth);

    SDL_RenderSetLogicalSize(
      emu->renderer,
      (int)w,
      (int)h
    );
    emu->texture = SDL_CreateTexture(
      emu->renderer,
      emu->sdlFormat,
      SDL_TEXTUREACCESS_STREAMING,
      (int)w,
      (int)h
    );
  }

  if(format == f8888) {
    for(int i = 0; i < w * h * depth; i += (int)depth) {
      emu->framebuffer[i] = mem->rdram[BYTE_ADDR((i + origin) & RDRAM_DSIZE)];
      emu->framebuffer[i + 1] = mem->rdram[BYTE_ADDR((i + 1 + origin) & RDRAM_DSIZE)];
      emu->framebuffer[i + 2] = mem->rdram[BYTE_ADDR((i + 2 + origin) & RDRAM_DSIZE)];
      emu->framebuffer[i + 3] = mem->rdram[BYTE_ADDR((i + 3 + origin) & RDRAM_DSIZE)];
    }
  } else if (format == f5553) {
    for(int i = 0; i < w * h * depth; i += (int)depth) {
      emu->framebuffer[i] = mem->rdram[HALF_ADDR((i + origin) & RDRAM_DSIZE)];
      emu->framebuffer[i + 1] = mem->rdram[HALF_ADDR((i + 1 + origin) & RDRAM_DSIZE)];
    }
  }

  SDL_UpdateTexture(
    emu->texture,
    NULL,
    emu->framebuffer,
    (int)w * depth
  );

  SDL_RenderCopy(emu->renderer, emu->texture, NULL, NULL);
  SDL_RenderPresent(emu->renderer);
}

void emu_run(emu_t* emu) {
  core_t* core = &emu->core;
  bool running = true;

  while(running) {
    if(core->running) {
      run_frame(core);
      emu_present(emu);
    } else {
      SDL_SetRenderDrawColor(emu->renderer, 0, 0, 0, 0);
      SDL_RenderClear(emu->renderer);
      SDL_RenderPresent(emu->renderer);
    }

    SDL_Event e;
    SDL_PollEvent(&e);
    switch(e.type) {
    case SDL_QUIT:
      running = false;
      break;
    case SDL_KEYDOWN:
      switch(e.key.keysym.sym) {
      case SDLK_o: {
        nfdfilteritem_t filter = { "Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64" };
        nfdresult_t result = NFD_OpenDialog(&emu->romFile, &filter, 1, ".");
        if(result == NFD_OKAY) {
          core->running = false;
          init_core(core);
          core->running = load_rom(&core->mem, emu->romFile);
        }
      } break;
      }
      break;
    }
  }

  NFD_Quit();
  destroy_emu(emu);
}