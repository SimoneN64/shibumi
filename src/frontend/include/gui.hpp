#pragma once
#include <core.h>
#include <gui/disasm.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <nfd.hpp>
#include <atomic>
#include <pthread.h>
#include <ctime>
#include <gui/logger.hpp>

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
  bool show_disasm = false, show_regs = false;
  bool show_metrics = false, show_logs = true;
  float log_pos_y = 0;
  std::string old_message = "NULL";
  message_type old_message_type = INFO;
	SDL_Window* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded = false, running = true;
  SDL_GLContext gl_context;
  pthread_t emu_thread;
  Logger logger;
  clock_t delta;
  double fps = 60.0, frametime = 16.0;
  std::atomic_bool emu_quit = false;
  core_t core;
  gl_data_t gl_data;
  Disasm disassembler;
  u8* framebuffer;
  
  Gui(const char* title);
  ~Gui();
  void MainLoop();
  void DestroyGui();
  void OpenFile();
  void MainMenubar();
  void DebuggerWindow();
  void RegistersView();
  void UpdateTexture();
  void Reset();
  void Stop();
  void Start();
};