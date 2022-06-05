#include <emu.h>
#include <mem.h>

void destroy_emu(emu_t* emu) {
  SDL_DestroyTexture(emu->texture);
  SDL_DestroyRenderer(emu->renderer);
  SDL_DestroyWindow(emu->window);
  SDL_Quit();
  NFD_Quit();
}

void init_emu(emu_t* emu) {
  init_core(&emu->core);

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
  emu->texture = SDL_CreateTexture(
    emu->renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    320,
    240
  );
  SDL_RenderSetLogicalSize(emu->renderer, 320, 240);
  NFD_Init();
}

void emu_present(emu_t* emu) {
  mem_t* mem = &emu->core.mem;
  const u32 w = mem->mmio.vi.width, h = 0.75 * w;
  const u32 origin = mem->mmio.vi.origin & 0xFFFFFF;
  const u8 format = mem->mmio.vi.status.format;
  u8 depth = format == f8888 ? 4 : format == f5553 ? 2 : 0;
  u8* temp = calloc(RDRAM_SIZE - origin, 1);

  if(format == f8888) {
    memcpy(temp, &mem->rdram[origin], w * h * depth);
  } else if (format == f5553) {
    for(int i = 0; i < w * h * depth; i += (int)depth) {
      temp[i] = mem->rdram[(i + origin) & RDRAM_DSIZE];
      temp[i + 1] = mem->rdram[(i + 1 + origin) & RDRAM_DSIZE];
    }
  }

  bool reconstructTexture = false;
  if(emu->currentW != w || emu->currentH != h) {
    emu->currentW = w;
    emu->currentH = h;

    reconstructTexture = true;
  }

  if(emu->currentFormat != format) {
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
    SDL_DestroyTexture(emu->texture);
    SDL_RenderSetLogicalSize(emu->renderer, (int)w, (int)h);
    emu->texture = SDL_CreateTexture(emu->renderer, emu->sdlFormat, SDL_TEXTUREACCESS_STREAMING, (int)w, (int)h);
  }

  int pitch = 0;
  void *pixels = NULL;
  SDL_LockTexture(emu->texture, NULL, &pixels, &pitch);
  SDL_ConvertPixels(w, h, emu->sdlFormat, temp, w * depth, emu->sdlFormat, pixels, pitch);
  SDL_UnlockTexture(emu->texture);

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