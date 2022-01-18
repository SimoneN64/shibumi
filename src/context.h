#pragma once
#include <SDL2/SDL.h>
#include <vi.h>
#define W 1024
#define H 768

typedef struct mem_t mem_t;

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u8 currentFormat;
  u32 currentW, currentH;
  u8 *framebuffer;
  SDL_PixelFormatEnum sdlFormat;
} context_t;

void destroy_context(context_t* context);
void init_context(context_t* context);
void context_present(context_t* context, mem_t* mem);