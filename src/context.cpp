#include <context.hpp>
#include <mem.h>

Context::~Context() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

Context::Context() {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow(
    "shibumi",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    W,
    H,
    SDL_WINDOW_RESIZABLE
  );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, currentW, currentH);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  texture = SDL_CreateTexture(renderer, sdlFormat, SDL_TEXTUREACCESS_STREAMING, currentW, currentH);
  framebuffer = (unsigned char*)calloc(currentW * currentH, depth);
}

void Context::Present(mem_t& mem) {
  u32 w = mem.mmio.vi.width, h = 0.75 * w;
  u32 origin = mem.mmio.vi.origin & 0xFFFFFF;
  u8 format = mem.mmio.vi.status.format;
  bool reconstructTexture = false;
  bool resChanged = currentW != w || currentH != h;
  bool formatChanged = currentFormat != format;
  if(resChanged) {
    currentW = w;
    currentH = h;

    reconstructTexture = true;
  }

  if(formatChanged) {
    currentFormat = format;
    if(format == f5553) {
      sdlFormat = SDL_PIXELFORMAT_RGBA5551;
      depth = 2;
    } else if(format == f8888) {
      sdlFormat = SDL_PIXELFORMAT_RGBA32;
      depth = 4;
    }

    reconstructTexture = true;
  }

  if(reconstructTexture) {
    SDL_DestroyTexture(texture);
    framebuffer = (u8*)realloc(framebuffer, w * h * depth);
    texture = SDL_CreateTexture(renderer, sdlFormat, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_RenderSetLogicalSize(renderer, w, h);
  }

  if(format == f8888) {
    memcpy(framebuffer, &mem.rdram[origin & RDRAM_DSIZE], currentW * currentH * depth);
  } else if (format == f5553) {
    for(int i = 0; i < currentW * currentH * depth; i += depth) {
      framebuffer[i] = mem.rdram[HALF_ADDR(origin + i & RDRAM_DSIZE)];
      framebuffer[i + 1] = mem.rdram[HALF_ADDR(origin + 1 + i & RDRAM_DSIZE)] | (1 << 16);
    }
  }

  SDL_UpdateTexture(texture, nullptr, framebuffer, currentW * depth);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}