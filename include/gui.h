#pragma once
#include <core.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#define f5_to_8(x) (((x) << 3) | ((x) >> 2))
#define f1_to_8(x) (((x) << 7) | ((x) >> 7))

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  u8* framebuffer;
} gui_t;

void init_gui(gui_t* gui);
void run(gui_t* gui, core_t* core);
void update_framebuffer(gui_t* gui, core_t* core, u32* old_w, u32* old_h, u8* old_format);