#include <capstone.h>
#include <core.h>
#include <gui.h>
#include <utils.h>
#include <string.h>

void* core_callback(void* vpargs) {
  gui_t* gui = (gui_t*)vpargs;
  while(!atomic_load(&gui->emu_quit)) {
    clock_t begin = clock();
    run_frame(&gui->core);
    clock_t end = clock();
    gui->delta += end - begin;
  }

  return NULL;
}

void init_gui(gui_t* gui, const char* title) {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    logfatal("Error: %s\n", SDL_GetError());
  }

  gui->rom_loaded = false;
  gui->running = true;
  gui->show_debug_windows = true;

  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  
  gui->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  gui->gl_context = SDL_GL_CreateContext(gui->window);
  SDL_GL_MakeCurrent(gui->window, gui->gl_context);
  SDL_GL_SetSwapInterval(0); // Enable vsync

  ImFontAtlas* firacode_font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* firacode_font = ImFontAtlas_AddFontFromFileTTF(firacode_font_atlas, "resources/FiraCode-VariableFont_wght.ttf", 16, NULL, NULL);

  gui->ctx = igCreateContext(firacode_font_atlas);
  gui->io = igGetIO();
  gui->io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  gui->io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui_ImplSDL2_InitForOpenGL(gui->window, gui->gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiStyle* style = igGetStyle();
  igStyleColorsDark(style);
  
  style->WindowRounding = 10;
  
  init_core(&gui->core);
  init_disasm(&gui->debugger);

  // InitMemoryEditor(&gui->memory_editor, read8_ignore_tlb_and_maps, NULL);

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

static inline void resize_callback(ImGuiSizeCallbackData* data) {
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
  image_size.y = y - 30;
}

void main_loop(gui_t* gui) {
  ImGuiIO* io = igGetIO();
  ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
  unsigned int frames = 0;
  while(gui->running) {    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch(event.type) {
        case SDL_QUIT: gui->running = false; break;
        case SDL_WINDOWEVENT:
          if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
            gui->running = false;
          }
          break;  
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_o: open_file(gui); break;
            case SDLK_p:
              if(gui->rom_loaded) {
                gui->core.running = !gui->core.running;
              }
              break;
            case SDLK_ESCAPE: gui->running = false; break;
          }
          break;
      }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
    
    debugger_window(gui);
    
    igSetNextWindowSizeConstraints((ImVec2){0, 0}, (ImVec2){__FLT_MAX__, __FLT_MAX__}, resize_callback, NULL);
    igBegin("Display", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
    main_menubar(gui);
    update_texture(gui);
    ImVec2 window_size;
    igGetWindowSize(&window_size);
    ImVec2 result = {(window_size.x - image_size.x) * 0.5, (window_size.y - image_size.y + 15) * 0.5};
    igSetCursorPos(result);
    igImage((ImTextureID)((intptr_t)gui->id), image_size, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
    igEnd();

    frames++;

    if(clock_to_ms(gui->delta)>1000.0) {
      gui->fps = (double)frames * 0.5 + gui->fps *0.5;
      frames = 0;
      gui->delta -= CLOCKS_PER_SEC;
      gui->frametime = 1000.0/((gui->fps == 0) ? 0.001 : gui->fps);
    }

    igRender();
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    
    SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    igUpdatePlatformWindows();
    igRenderPlatformWindowsDefault(NULL, NULL);
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

    SDL_GL_SwapWindow(gui->window);
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

  memory_regions_t* memory_regions = &gui->core.mem.memory_regions;

  if(format == f8888) {
    memcpy(gui->framebuffer, &memory_regions->rdram[origin & RDRAM_DSIZE], w * h * gui->gl_data.depth);
  } else {
    for(int i = 0; i < w * h * gui->gl_data.depth; i += gui->gl_data.depth) {
      gui->framebuffer[i] = memory_regions->rdram[HALF_ADDR(origin + i & RDRAM_DSIZE)];
      gui->framebuffer[i + 1] = memory_regions->rdram[HALF_ADDR(origin + 1 + i & RDRAM_DSIZE)];
    }
  }

  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, gui->gl_data.glFormat, gui->framebuffer);
}

void main_menubar(gui_t *gui) {
  ImVec2 window_size;
  igGetWindowSize(&window_size);
  if(igBeginMenuBar()) {
    if(igBeginMenu("File", true)) {
      if(igMenuItem_Bool("Open", "O", false, true)) {
        open_file(gui);
      }

      if(igMenuItem_Bool("Exit", "Esc", false, true)) {
        gui->running = false;
      }
      igEndMenu();
    }

    if(igBeginMenu("Emulation", true)) {
      char* pause_text = "Pause";
      if(!gui->core.running && gui->rom_loaded) {
        pause_text = "Resume";
      }

      if(igMenuItem_Bool(pause_text, "P", false, gui->rom_loaded)) {
        gui->core.running = !gui->core.running;
        if(gui->core.running) {
          gui->core.stepping = false;
        }
      }

      if(igMenuItem_Bool("Stop", NULL, false, gui->rom_loaded)) {
        stop(gui);
      }

      if(igMenuItem_Bool("Reset", NULL, false, gui->rom_loaded)) {
        reset(gui);
      }

      igEndMenu();
    }

    if(igBeginMenu("Settings", true)) {
      igMenuItem_BoolPtr("Show debug windows", "", &gui->show_debug_windows, true);
      igEndMenu();
    }

    igSameLine(window_size.x - 220, -1);
    igText(gui->rom_loaded ? "[ %.2f fps ][ %.2f ms ]" : "[ NaN fps ][ NaN ms ]", gui->fps, gui->frametime);
    igSameLine(window_size.x - 30, -1);
    if(igBeginMenu("X", true)) {
      gui->running = false;
      igEndMenu();
    }

    igEndMenuBar();
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
  igBegin("Disassembly", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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

  static int num_instr = 0;

  char run_n_instr_str[20];
  sprintf(run_n_instr_str, "Run %d instr", num_instr);
  run_n_instr_str[19] = '\0';

  if(igButton(run_n_instr_str, (ImVec2){ (window_size.x / 3) - 10, 20 })) {
    gui->core.stepping = true;
    for(int i = 0; i < num_instr; i++) {
      step(&gui->core.cpu, &gui->core.mem);
    }
  }
  
  igSameLine(window_size.x / 3, 5);
  igSetNextItemWidth(window_size.x / 10);
  igInputInt("Instruction count to run", &num_instr, 0, 0, ImGuiInputTextFlags_CharsNoBlank);
  
  static int addr = 0xFFFFFFFF;

  igInputInt("Address", &addr, 0, 0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank);

  igSameLine(window_size.x - (window_size.x / 4), 5);

  if(igButton("Set breakpoint", (ImVec2){(window_size.x / 4) - 10, 20})) {
    gui->core.break_addr = addr;
  }

  igSpacing();

  igBeginChild_Str("frame", window_size, false, 0);
  if(gui->debugger.count > 0) {
    for(size_t j = 0; j < gui->debugger.count; j++) {
      const float font_size = igGetFontSize() * strlen(gui->debugger.insn[j].op_str) / 2;
      switch(j) {
      case 12 ... 14:
        igTextColored(colors_disasm[j & 3], "0x%"PRIx64":\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic);
        igSameLine(window_size.x - font_size - 10, -1);
        igTextColored(colors_disasm[j & 3], "%s", gui->debugger.insn[j].op_str);
        break;
      default:
        igText("0x%"PRIx64":\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic);
        igSameLine(window_size.x - font_size - 10, -1);
        igText("%s", gui->debugger.insn[j].op_str);
      }
    }

    cs_free(gui->debugger.insn, gui->debugger.count);
  } else {
    igText(gui->rom_loaded ? "Could not disassemble instruction!" : "No game loaded!");
  }
  igEndChild();
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
  if(gui->show_debug_windows) {
    disassembly(gui);
    // Draw(&gui->memory_editor, &gui->core.mem, "Memory Editor", 800, 0);
    registers_view(gui);
  }
}

void destroy_gui(gui_t* gui) {
  gui->emu_quit = true;
  pthread_join(gui->emu_thread_id, NULL);
  destroy_disasm(&gui->debugger);
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  igDestroyContext(gui->ctx);
  SDL_GL_DeleteContext(gui->gl_context);
  SDL_DestroyWindow(gui->window);
  SDL_Quit();
}

void open_file(gui_t* gui) {
  gui->rom_file = "";
  nfdfilteritem_t filter = { "Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64" };
  nfdresult_t result = NFD_OpenDialog(&gui->rom_file, &filter, 1, EMU_DIR);
  if(result == NFD_OKAY) {
    reset(gui);
  }
}

void start(gui_t* gui) {
  gui->rom_loaded = load_rom(&gui->core.mem, gui->rom_file);
  gui->emu_quit = !gui->rom_loaded;
  gui->core.running = gui->rom_loaded;
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