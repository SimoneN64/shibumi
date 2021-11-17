#pragma once
#include <core.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <imgui_memory_editor.h>
#include <imgui_logger/imgui_logger.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
// #include <cimgui_memory_editor.h>
#include <nfd.hpp>
#include <atomic>
#include <pthread.h>
#include <ctime>
#include <log.h>
#include <string>

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

INLINE ImU32 colors_print(message_type type) {
  switch(type) {
    case INFO: return 0xffffffff;
    case WARNING: return 0xff00ccff;
    case FATAL: return 0xff0000ff;
  }
}

static const std::string message_type_strings[3] = {"[INFO]", "[WARNING]", "[FATAL]"};

struct Gui {
  ImGuiContext* ctx;
  bool show_disasm = false, show_regs = false;
  bool show_memory_editor = false, show_metrics = false;
  bool show_logs = true;
  float log_pos_y = 0;
  std::string old_message = "NULL";
  message_type old_message_type = INFO;
	SDL_Window* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded = false, running = true;
  SDL_GLContext gl_context;
  pthread_t emu_thread;
  MemoryEditor memory_editor;
  Logger logger;
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
  void LogWindow();
  void Disassembly();
  void RegistersView();
  void UpdateTexture();
  void Reset();
  void Stop();
  void Start();
};