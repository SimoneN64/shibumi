#pragma once
#include <core.h>
#include <SDL2/SDL.h>

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u32* framebuffer;
} gui_t;

void init_gui(gui_t* gui);
void run(gui_t* gui, core_t* core);