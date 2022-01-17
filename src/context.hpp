#pragma once
#include <SDL2/SDL.h>
#include <vi.h>
#define W 1024
#define H 768

struct mem_t;

struct Context {
  ~Context();
  Context();
  void Present(mem_t& mem);
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  unsigned char currentFormat = 3;
  int currentW = 320, currentH = 240, depth = 4;
  unsigned char* framebuffer;
  SDL_PixelFormatEnum sdlFormat = SDL_PIXELFORMAT_RGBA32;
};