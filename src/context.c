#include <context.h>
#include <mem.h>

void destroy_context(context_t* context) {
  SDL_DestroyTexture(context->texture);
  SDL_DestroyRenderer(context->renderer);
  SDL_DestroyWindow(context->window);
  SDL_Quit();
  free(context->framebuffer);
}

void init_context(context_t* context) {
  context->currentW = 320;
  context->currentH = 240;
  context->sdlFormat = SDL_PIXELFORMAT_RGBA32;
  context->currentFormat = f8888;

  SDL_Init(SDL_INIT_VIDEO);
  context->window = SDL_CreateWindow(
    "shibumi",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    W,
    H,
    SDL_WINDOW_RESIZABLE
  );
  context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(context->renderer, (int)context->currentW, (int)context->currentH);
  context->texture = SDL_CreateTexture(
    context->renderer,
    context->sdlFormat,
    SDL_TEXTUREACCESS_STREAMING,
    (int)context->currentW,
    (int)context->currentH
 );
  context->framebuffer = (u8*)calloc(context->currentW * context->currentH, 4);
}

void context_present(context_t* context, mem_t* mem) {
  u32 w = mem->mmio.vi.width, h = 0.75 * w;
  u32 origin = mem->mmio.vi.origin & 0xFFFFFF;
  u8 format = mem->mmio.vi.status.format;
  u8 depth = format == f8888 ? 4 : format == f5553 ? 2 : 0;
  bool reconstructTexture = false;
  bool resChanged = context->currentW != w || context->currentH != h;
  bool formatChanged = context->currentFormat != format;
  if(resChanged) {
    context->currentW = w;
    context->currentH = h;

    reconstructTexture = true;
  }

  if(formatChanged) {
    context->currentFormat = format;
    if(format == f5553) {
      context->sdlFormat = SDL_PIXELFORMAT_RGBA5551;
      depth = 2;
    } else if(format == f8888) {
      context->sdlFormat = SDL_PIXELFORMAT_RGBA32;
      depth = 4;
    }

    reconstructTexture = true;
  }

  if(reconstructTexture) {
    SDL_DestroyTexture(context->texture);
    context->framebuffer = (u8*)realloc(
      context->framebuffer,
      w * h * depth
    );
    memset(context->framebuffer, 0, w * h * depth);
    SDL_RenderSetLogicalSize(
      context->renderer,
      (int)w,
      (int)h
    );
    context->texture = SDL_CreateTexture(
      context->renderer,
      context->sdlFormat,
      SDL_TEXTUREACCESS_STREAMING,
      (int)w,
      (int)h
    );
  }

  if(format == f8888) {
    for(int i = 0; i < w * h * depth; i += (int)depth) {
      context->framebuffer[i] = mem->rdram[BYTE_ADDR((i + origin) & RDRAM_DSIZE)];
      context->framebuffer[i + 1] = mem->rdram[BYTE_ADDR((i + 1 + origin) & RDRAM_DSIZE)];
      context->framebuffer[i + 2] = mem->rdram[BYTE_ADDR((i + 2 + origin) & RDRAM_DSIZE)];
      context->framebuffer[i + 3] = mem->rdram[BYTE_ADDR((i + 3 + origin) & RDRAM_DSIZE)];
    }
  } else if (format == f5553) {
    for(int i = 0; i < w * h * depth; i += (int)depth) {
      context->framebuffer[i] = mem->rdram[HALF_ADDR((i + origin) & RDRAM_DSIZE)];
      context->framebuffer[i + 1] = mem->rdram[HALF_ADDR((i + 1 + origin) & RDRAM_DSIZE)];
    }
  }

  SDL_UpdateTexture(
    context->texture,
    NULL,
    context->framebuffer,
    w * depth
  );
}