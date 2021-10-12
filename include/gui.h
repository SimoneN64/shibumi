#pragma once
#pragma once
#include <glad.h>
#ifdef _WIN32
#include <glfw/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include <pthread.h>
#include <core.h>
#include <stdatomic.h>
#include <capstone/capstone.h>
#include <nfd.h>

#define N64_ASPECT_RATIO (float)4 / 3

static const ImVec2 ZERO = {.x = 0, .y = 0};
static const ImVec2 ONE = {.x = 1, .y = 1};
static const ImVec2 MAX = {.x = __FLT_MAX__, .y = __FLT_MAX__};
static const ImVec4 ZERO4 = {.x = 0, .y = 0, .z = 0, .w = 0};
static const ImVec4 FULL4 = {.x = 1, .y = 1, .z = 1, .w = 1};

static const ImVec4 colors_disasm[3] = {{.x = 1, .y = 0.000, .z = 0, .w = 1},  // RED
                                        {.x = 1, .y = 0.619, .z = 0, .w = 1},  // ORANGE
                                        {.x = 1, .y = 0.988, .z = 0, .w = 1}}; // YELLOW

typedef struct {
  u32 old_w, old_h;
  u8 old_format;
  int glFormat;
  u8 depth;
} gl_data_t;

typedef struct {
  ImGuiContext* ctx;
  ImGuiIO* io;
	GLFWwindow* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded;
  pthread_t emu_thread_id;
  atomic_bool emu_quit;
  core_t core;
  gl_data_t gl_data;
  disasm_t debugger;
  u8* framebuffer;
} gui_t;

void init_gui(gui_t* gui, const char* title);
void main_loop(gui_t* gui);
void destroy_gui(gui_t* gui);
void open_file(gui_t* gui);
void main_menubar(gui_t* gui);
void debugger_window(gui_t* gui);
void disassembly(gui_t* gui);
void registers_view(gui_t* gui);
void update_texture(gui_t* gui);
void reset(gui_t* gui);