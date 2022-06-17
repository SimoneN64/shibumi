#pragma once
#include <SDL2/SDL.h>
#include <core.h>
#include <nfd.h>
#define W 1024
#define H 768

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u8 currentFormat;
  u32 currentW, currentH;
  SDL_PixelFormatEnum sdlFormat;
  core_t core;
  nfdchar_t* romFile;
  u8* framebuffer;
  SDL_GameController* controller;
  bool has_gamepad;
} emu_t;

void destroy_emu(emu_t* emu);
void init_emu(emu_t* emu);
void emu_run(emu_t* emu);
void emu_present(emu_t* emu);
