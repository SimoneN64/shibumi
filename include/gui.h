#pragma once
#include <core.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
// #include <cimgui_memory_editor.h>
#include <nfd.hpp>
#include <atomic>
#include <thread>
#include <ctime>

#define N64_ASPECT_RATIO (float)4 / 3

static const ImVec4 colors_disasm[3] = {ImVec4(1.0, 0.000, 0.0, 1.0),  // RED
                                        ImVec4(1.0, 0.619, 0.0, 1.0),  // ORANGE
                                        ImVec4(1.0, 0.988, 0.0, 1.0)}; // YELLOW

struct gl_data_t {
  u32 old_w, old_h;
  u8 old_format = 14;
  int glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
  u8 depth = 2;
};

struct Gui {
  ImGuiContext* ctx;
  bool show_debug_windows = false;
	SDL_Window* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded = false, running = true;
  SDL_GLContext gl_context;
  std::thread emu_thread;
  // MemoryEditor memory_editor;
  clock_t delta;
  double fps = 60.0, frametime = 16.0;
  std::atomic_bool emu_quit = false;
  core_t core;
  gl_data_t gl_data;
  disasm_t debugger;
  u8* framebuffer;
  
  Gui(const char* title);
  ~Gui();
  void MainLoop();
  void DestroyGui();
  void OpenFile();
  void MainMenubar();
  void DebuggerWindow();
  void Disassembly();
  void RegistersView();
  void UpdateTexture();
  void Reset();
  void Stop();
  void Start();
};