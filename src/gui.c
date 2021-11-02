#include <capstone/capstone.h>
#include <core.h>
#include <gui.h>
#include <utils/log.h>
#include <string.h>

static void glfw_error_callback(int error, const char* description) {
  logfatal("Glfw Error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void* core_callback(void* vpargs) {
  gui_t* gui = (gui_t*)vpargs;
  while(!atomic_load(&gui->emu_quit)) {
    run_frame(&gui->core);
  }

  return NULL;
}

void init_gui(gui_t* gui, const char* title) {
  if(glfwInit() == GLFW_FALSE) {
    logfatal("Couldn't initialize GLFW\n");
  }

  gui->rom_loaded = false;

  const char* glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwSetErrorCallback(glfw_error_callback);
  
  const GLFWvidmode *details = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int w = details->width - (details->width / 4), h = details->height - (details->height / 4);
  gui->window = glfwCreateWindow(w, h, title, NULL, NULL);
  glfwSetWindowPos(gui->window, details->width / 2 - w / 2, details->height / 2 - h / 2);
  glfwSetFramebufferSizeCallback(gui->window, framebuffer_size_callback);

  glfwMakeContextCurrent(gui->window);
  glfwSwapInterval(0);

  if(!gladLoadGL()) {
    logfatal("Failed to initialize OpenGL loader!\n");
  }

  ImFontAtlas* firacode_font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* firacode_font = ImFontAtlas_AddFontFromFileTTF(firacode_font_atlas, "resources/FiraCode-VariableFont_wght.ttf", 16, NULL, NULL);

  gui->ctx = igCreateContext(firacode_font_atlas);
  gui->io = igGetIO();

  ImGui_ImplGlfw_InitForOpenGL(gui->window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 10.0;
  igStyleColorsDark(NULL);
  
  init_core(&gui->core);
  init_disasm(&gui->debugger);

  gui->framebuffer = calloc(320 * 240, 4);

  glGenTextures(1, &gui->id);
  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 240, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, gui->framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  gui->gl_data.depth = 2;
  gui->gl_data.glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
  gui->gl_data.old_format = 0xE;
  
	NFD_Init();

  pthread_create(&gui->emu_thread_id, NULL, core_callback, (void*)gui);
}

ImVec2 image_size;

static void resize_callback(ImGuiSizeCallbackData* data) {
  ImVec2 window_size;
  igGetWindowSize(&window_size);
  float x = window_size.x - 15, y = window_size.y - 15;
  float current_aspect_ratio = x / y;
  
  if(N64_ASPECT_RATIO > current_aspect_ratio) {
    y = x / (N64_ASPECT_RATIO);
  } else {
    x = y * (N64_ASPECT_RATIO);
  }

  image_size.x = x;
  image_size.y = y;
}

void main_loop(gui_t* gui) {;
  while(!glfwWindowShouldClose(gui->window)) {    
    update_texture(gui);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    if (glfwGetKey(gui->window, GLFW_KEY_O) == GLFW_PRESS) {
      open_file(gui);
    } else if (glfwGetKey(gui->window, GLFW_KEY_P) == GLFW_PRESS && gui->rom_loaded) {
      gui->core.running = !gui->core.running;
    } else if (glfwGetKey(gui->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(gui->window, GLFW_TRUE);
    }
    
    main_menubar(gui);
    debugger_window(gui);
    
    igSetNextWindowSizeConstraints((ImVec2){0, 0}, (ImVec2){__FLT_MAX__, __FLT_MAX__}, resize_callback, NULL);
    igBegin("Display", NULL, ImGuiWindowFlags_NoTitleBar);
    ImVec2 window_size;
    igGetWindowSize(&window_size);
    ImVec2 result = {.x = (window_size.x - image_size.x) * 0.5, .y = (window_size.y - image_size.y) * 0.5};
    igSetCursorPos(result);
    igImage((ImTextureID)((intptr_t)gui->id), image_size, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
    igEnd();
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.227, 0.345, 0.454, 1.00);
    
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

    glfwSwapBuffers(gui->window);
    glfwPollEvents();
  }
}

void update_texture(gui_t* gui) {
  u32 w = gui->core.mem.mmio.vi.width, h = 0.75 * w;
  u32 origin = gui->core.mem.mmio.vi.origin & 0xFFFFFF;
  u8 format = gui->core.mem.mmio.vi.status.format;
  bool reconstruct_texture = false;
  bool res_changed = gui->gl_data.old_w != w || gui->gl_data.old_h != h;
  bool format_changed = gui->gl_data.old_format != format;

  if(res_changed) {
    gui->gl_data.old_w = w;
    gui->gl_data.old_h = h;

    reconstruct_texture = true;
  }

  if(format_changed) {
    gui->gl_data.old_format = format;
    if(format == f5553) {
      gui->gl_data.glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
      gui->gl_data.depth = 2;
    } else if (format == f8888) {
      gui->gl_data.glFormat = GL_UNSIGNED_INT_8_8_8_8;
      gui->gl_data.depth = 4;
    }

    reconstruct_texture = true;
  }

  if(reconstruct_texture) {
    gui->framebuffer = realloc(gui->framebuffer, w * h * gui->gl_data.depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, gui->gl_data.glFormat, gui->framebuffer);
  }

  if(format == f8888) {
    memcpy(gui->framebuffer, &gui->core.mem.rdram[origin & RDRAM_DSIZE], w * h * gui->gl_data.depth);
  } else {
    for(int i = 0; i < w * h * gui->gl_data.depth; i += gui->gl_data.depth) {
      gui->framebuffer[i] = gui->core.mem.rdram[HALF_ADDR(origin + i & RDRAM_DSIZE)];
      gui->framebuffer[i + 1] = gui->core.mem.rdram[HALF_ADDR(origin + 1 + i & RDRAM_DSIZE)];
    }
  }

  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, gui->gl_data.glFormat, gui->framebuffer);
}

void main_menubar(gui_t *gui) {
  if(igBeginMainMenuBar()) {
    if(igBeginMenu("File", true)) {
      if(igMenuItem_Bool("Open", "O", false, true)) {
        open_file(gui);
      }
      if(igMenuItem_Bool("Exit", "Esc", false, true)) {
        glfwSetWindowShouldClose(gui->window, GLFW_TRUE);
      }
      igEndMenu();
    }
    if(igBeginMenu("Emulation", true)) {
      if(igMenuItem_Bool(gui->core.running ? "Pause" : "Resume", "P", false, gui->rom_loaded)) {
        gui->core.running = !gui->core.running;
        gui->core.paused = !gui->core.paused;
      }
      if(igMenuItem_Bool("Stop", NULL, false, gui->rom_loaded)) {
        stop(gui);
      }
      if(igMenuItem_Bool("Reset", NULL, false, gui->rom_loaded)) {
        reset(gui);
      }
      igEndMenu();
    }
    igEndMainMenuBar();
  }
}

void disassembly(gui_t *gui) {
  u32 instructions[25] = {};
  u32 pc = gui->core.cpu.regs.pc;
  u32 pointer = pc - (14 * 4);
  pointer -= (pointer & 3); // align the pointer

  if(gui->rom_loaded) {
    for(int i = 0; i < 25; i++) {
      instructions[i] = read32(&gui->core.mem, pointer + i * 4);
    }
  } else {
    memset(instructions, 0xFFFFFFFF, 100);
  }

  u8 code[100];
  memcpy(code, instructions, 100);
  
  gui->debugger.count = cs_disasm(gui->debugger.handle, code, sizeof(code), pointer, 25, &gui->debugger.insn);
  igBegin("Disassembly", NULL, 0);
  ImVec2 window_size;
  igGetWindowSize(&window_size);

  if(igButton("Step", (ImVec2){ (window_size.x / 2) - 10, 20 })) {
    gui->core.stepping = true;
    step(&gui->core.cpu, &gui->core.mem);
  }

  igSameLine(window_size.x / 2, 5);
  if(igButton("Run frame", (ImVec2){ (window_size.x / 2) - 10, 20 })) {
    gui->core.stepping = true;
    for(int i = 0; i < 100000; i++) {
      step(&gui->core.cpu, &gui->core.mem);
    }
  }

  s64 addr = -1;

  if(igButton("Set breakpoint", (ImVec2){(window_size.x / 4) - 10, 20})) {
    if(addr != -1) {
      gui->core.breakpoint = true;
      gui->core.break_addr = addr;
    }
  }

  igSameLine(window_size.x / 4, 5);

  char buf[9];
  igInputText("Address", buf, 9, ImGuiInputTextFlags_CharsHexadecimal, NULL, NULL);

  igSpacing();

  if(gui->debugger.count > 0) {
    for(size_t j = 0; j < gui->debugger.count; j++) {
      const float font_size = igGetFontSize() * strlen(gui->debugger.insn[j].op_str) / 2;
      switch(j) {
      case 12 ... 14:
        igTextColored(colors_disasm[j & 3], "0x%"PRIx64":\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic);
        igSameLine(window_size.x - font_size, -1);
        igTextColored(colors_disasm[j & 3], "%s", gui->debugger.insn[j].op_str);
        break;
      default:
        igText("0x%"PRIx64":\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic);
        igSameLine(window_size.x - font_size, -1);
        igText("%s", gui->debugger.insn[j].op_str);
      }
    }

    cs_free(gui->debugger.insn, gui->debugger.count);
  } else {
    igText(gui->rom_loaded ? "Could not disassemble instruction!" : "No game loaded!");
  }
  igEnd();
}

void registers_view(gui_t *gui) {
  registers_t* regs = &gui->core.cpu.regs;
  igBegin("Registers view", NULL, 0);
  for(int i = 0; i < 32; i+=4) {
    igText("%4s: %08X %4s: %08X %4s: %08X %4s: %08X", regs_str[i], regs->gpr[i], regs_str[i + 1], regs->gpr[i + 1], regs_str[i + 2], regs->gpr[i + 2], regs_str[i + 3], regs->gpr[i + 3]);
  }
  igSeparator();
  s64 pipe[3] = {regs->old_pc, regs->pc, regs->next_pc};
  for(int i = 0; i < 3; i++) {
    igText("pipe[%d]: %08X", i, pipe[i]);
  }
  igEnd();
}

void debugger_window(gui_t* gui) {
  disassembly(gui);
  registers_view(gui);
}

void destroy_gui(gui_t* gui) {
  gui->emu_quit = true;
  pthread_join(gui->emu_thread_id, NULL);
  destroy_disasm(&gui->debugger);
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(gui->ctx);
  glfwDestroyWindow(gui->window);
}

void open_file(gui_t* gui) {
  gui->rom_file = "";
	nfdfilteritem_t filter = { "Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64" };
	nfdresult_t result = NFD_OpenDialog(&gui->rom_file, &filter, 1, "roms/");
	if(result == NFD_OKAY) {
    reset(gui);
	}
}

void start(gui_t* gui) {
  gui->rom_loaded = load_rom(&gui->core.mem, gui->rom_file);
  gui->core.running = gui->rom_loaded;
  gui->emu_quit = !gui->rom_loaded;
  if(gui->rom_loaded) {
    pthread_create(&gui->emu_thread_id, NULL, core_callback, (void*)gui);
  }
}

void reset(gui_t* gui) {
  stop(gui);
  start(gui);
}

void stop(gui_t* gui) {
  gui->emu_quit = true;
  pthread_join(gui->emu_thread_id, NULL);
  init_core(&gui->core);
  gui->rom_loaded = false;
  gui->core.running = false;
}