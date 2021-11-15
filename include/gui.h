#pragma once
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
// #include <cimgui_memory_editor.h>
#include <pthread.h>
#include <core.h>
#include <time.h>
#include <stdatomic.h>
#include <nfd.h>

#define N64_ASPECT_RATIO (float)4 / 3

static const ImVec4 colors_disasm[3] = {{.x = 1, .y = 0.000, .z = 0, .w = 1},  // RED
                                        {.x = 1, .y = 0.619, .z = 0, .w = 1},  // ORANGE
                                        {.x = 1, .y = 0.988, .z = 0, .w = 1}}; // YELLOW

typedef struct {
  u32 old_w, old_h;
  u8 old_format;
  int glFormat;
  u8 depth;
} gl_data_t;

struct Gui {
  ImGuiContext* ctx;
  ImGuiIO& io;
  bool show_debug_windows;
	SDL_Window* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded, running;
  SDL_GLContext gl_context;
  pthread_t emu_thread_id;
  // MemoryEditor memory_editor;
  clock_t delta;
  double fps, frametime;
  atomic_bool emu_quit;
  core_t core;
  gl_data_t gl_data;
  disasm_t debugger;
  u8* framebuffer;
  
  Gui(const char* title);
  void main_loop();
  void destroy_gui();
  void open_file();
  void main_menubar();
  void debugger_window();
  void disassembly();
  void registers_view();
  void update_texture();
  void reset();
  void stop();
  void start();
};