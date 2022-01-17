#include <context.hpp>
#include <mem.h>

Context::~Context() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(framebuffer);
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
  texture = SDL_CreateTexture(renderer, sdlFormat, SDL_TEXTUREACCESS_STREAMING, currentW, currentH);
  framebuffer = (u8*)calloc(currentW * currentH, depth);
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
    if(currentFormat == f5553) {
      sdlFormat = SDL_PIXELFORMAT_RGBA5551;
      depth = 2;
    } else if(currentFormat == f8888) {
      sdlFormat = SDL_PIXELFORMAT_RGBA8888;
      depth = 4;
    }

    reconstructTexture = true;
  }

  if(reconstructTexture) {
    SDL_DestroyTexture(texture);
    framebuffer = (u8*)realloc(framebuffer, currentW * currentH * depth);
    SDL_RenderSetLogicalSize(renderer, currentW, currentH);
    texture = SDL_CreateTexture(renderer, sdlFormat, SDL_TEXTUREACCESS_STREAMING, currentW, currentH);
  }

  if(format == f8888) {
    framebuffer[4] = 0xff;
    memcpy(framebuffer, &mem.rdram[origin & RDRAM_DSIZE], currentW * currentH * depth);
    for(int i = 0; i < currentW * currentH * depth; i += depth) {
      framebuffer[i + 4] |= 0xff;
    }
  } else if (format == f5553) {
    framebuffer[1] |= 1;
    for(int i = 0; i < currentW * currentH * depth; i += depth) {
      framebuffer[i] = mem.rdram[HALF_ADDR((i + origin) & RDRAM_DSIZE)];
      framebuffer[i + 1] = mem.rdram[HALF_ADDR((i + 1 + origin) & RDRAM_DSIZE)] | (1 << 16);
    }
  }

  SDL_UpdateTexture(texture, nullptr, framebuffer, currentW * depth);
}