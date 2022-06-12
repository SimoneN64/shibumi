#include <emu.h>
#include <mem.h>
#include <access.h>

void destroy_emu(emu_t* emu) {
  SDL_DestroyTexture(emu->texture);
  SDL_DestroyRenderer(emu->renderer);
  SDL_DestroyWindow(emu->window);
  SDL_Quit();
  NFD_Quit();
  destroy_core(&emu->core);
}

void init_emu(emu_t* emu) {
  init_core(&emu->core);
  emu->currentW = 320;
  emu->currentH = 240;
  emu->sdlFormat = SDL_PIXELFORMAT_RGBA5551;
  emu->currentFormat = f5553;

  SDL_Init(SDL_INIT_VIDEO);
  emu->window = SDL_CreateWindow(
    "shibumi",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    W,
    H,
    SDL_WINDOW_RESIZABLE
  );
 
  emu->renderer = SDL_CreateRenderer(emu->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_RenderSetLogicalSize(emu->renderer, emu->currentW, emu->currentH);
  emu->texture = SDL_CreateTexture(
    emu->renderer,
    emu->sdlFormat,
    SDL_TEXTUREACCESS_STREAMING,
    emu->currentW,
    emu->currentH
  );

  emu->framebuffer = (u8*)calloc(emu->currentW * emu->currentH, 2);

  NFD_Init();
}

void emu_present(emu_t* emu) {
  mem_t* mem = &emu->core.mem;
  const u32 w = mem->mmio->vi.width, h = 0.75 * w;
  const u32 origin = mem->mmio->vi.origin & 0xFFFFFF;
  const u8 format = mem->mmio->vi.status.format;
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
      emu->sdlFormat = SDL_PIXELFORMAT_RGBA8888;
      depth = 4;
    }

    reconstructTexture = true;
  }

  if(reconstructTexture) {
    emu->framebuffer = realloc(emu->framebuffer, w * h * depth);
    SDL_DestroyTexture(emu->texture);
    SDL_RenderSetLogicalSize(emu->renderer, (int)w, (int)h);
    emu->texture = SDL_CreateTexture(emu->renderer, emu->sdlFormat, SDL_TEXTUREACCESS_STREAMING, (int)w, (int)h);
  }

  if(format == f5553) {
    for(int i = 0; i < w * h * 2; i+=2) {
      u16 val = raccess_16(mem->rdram, (i + origin) & RDRAM_DSIZE);
      memcpy(&emu->framebuffer[i], &val, 2);
    }
  } else {
    for(int i = 0; i < w * h * 4; i+=4) {
      u32 val = raccess_32(mem->rdram, (i + origin) & RDRAM_DSIZE);
      memcpy(&emu->framebuffer[i], &val, 4);
    }
  }
  SDL_UpdateTexture(emu->texture, NULL, emu->framebuffer, (int) w * depth);
  SDL_RenderCopy(emu->renderer, emu->texture, NULL, NULL);
}

void emu_run(emu_t* emu) {
  core_t* core = &emu->core;
  bool running = true;

  while(running) {
    if(core->running) {
      run_frame(core);
      emu_present(emu);
    }

    SDL_RenderPresent(emu->renderer);

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
}
