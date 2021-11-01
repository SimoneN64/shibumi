#include <capstone/capstone.h>
#include <core.h>
#include <gui.h>
#include <utils/log.h>
#include <string.h>

void* core_cb(void* vpargs) {
  gui_t* gui = (gui_t*)vpargs;
  while(!atomic_load(&gui->emu_quit)) {
    run_frame(&gui->core);
  }

  return NULL;
}

void init_gui(gui_t* gui, const char* title) {
  SDL_Init(SDL_INIT_VIDEO);

  gui->rom_loaded = false;

  SDL_DisplayMode vidmode;
  
  SDL_GetCurrentDisplayMode(0, &vidmode);
  int w = vidmode.w - (vidmode.w / 4), h = vidmode.h - (vidmode.h / 4);
  gui->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE);
  SDL_SetWindowPosition(gui->window, vidmode.w / 2 - w / 2, vidmode.h / 2 - h / 2);
  gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_ACCELERATED);
  gui->texture = SDL_CreateTexture(gui->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 320, 240);
  SDL_SetRenderDrawColor(gui->renderer, clear_colorR, clear_colorG, clear_colorG, 0xff);

  ImFontAtlas* firacode_font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* firacode_font = ImFontAtlas_AddFontFromFileTTF(firacode_font_atlas, "resources/FiraCode-VariableFont_wght.ttf", 16, NULL, NULL);

  gui->ctx = igCreateContext(firacode_font_atlas);
  gui->io = igGetIO();

  ImGui_ImplSDL2_InitForSDLRenderer(gui->window);
  ImGui_ImplSDLRenderer_Init(gui->renderer);

  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 10.0;
  igStyleColorsDark(NULL);
  
  init_core(&gui->core);
  init_disasm(&gui->debugger);

  gui->framebuffer = calloc(320 * 240, 4);

	NFD_Init();

  pthread_create(&gui->emu_thread_id, NULL, core_cb, (void*)gui);
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

void main_loop(gui_t* gui) {
  bool running = true;
  while(running) {    
    update_texture(gui);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();

    SDL_Event e;
    SDL_PollEvent(&e);

    switch(e.type) {
      case SDL_QUIT: running = false; break;
      case SDL_KEYDOWN:
      switch(e.key.keysym.sym) {
        case SDLK_o: case SDLK_ESCAPE:
          open_file(gui);
          break;
        case SDLK_p:
          if(gui->rom_loaded){
            gui->core.running = !gui->core.running;
          }
          break;
      }
    }
    
    main_menubar(gui, &running);
    debugger_window(gui);
    
    igSetNextWindowSizeConstraints((ImVec2){0, 0}, (ImVec2){__FLT_MAX__, __FLT_MAX__}, resize_callback, NULL);
    igBegin("Display", NULL, ImGuiWindowFlags_NoTitleBar);
    ImVec2 window_size;
    igGetWindowSize(&window_size);
    ImVec2 result = {.x = (window_size.x - image_size.x) * 0.5, .y = (window_size.y - image_size.y) * 0.5};
    igSetCursorPos(result);
    igImage((ImTextureID)((intptr_t)gui->texture), image_size, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
    igEnd();
    
    igRender();
    SDL_RenderClear(gui->renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(igGetDrawData());
    SDL_RenderPresent(gui->renderer);
  }
}

void update_texture(gui_t* gui) {
  u32 w = gui->core.mem.mmio.vi.width, h = 0.75 * w;
  u32 origin = gui->core.mem.mmio.vi.origin & 0xFFFFFF;
  u8 format = gui->core.mem.mmio.vi.status.format;
  bool reconstruct_texture = false;
  bool res_changed = gui->fb_data.old_w != w || gui->fb_data.old_h != h;
  bool format_changed = gui->fb_data.old_format != format;

  if(res_changed) {
    gui->fb_data.old_w = w;
    gui->fb_data.old_h = h;

    reconstruct_texture = true;
  }

  if(format_changed) {
    gui->fb_data.old_format = format;
    if(format == f5553) {
      gui->fb_data.texFormat = SDL_PIXELFORMAT_RGBA5551;
      gui->fb_data.depth = 2;
    } else if (format == f8888) {
      gui->fb_data.texFormat = SDL_PIXELFORMAT_RGBA8888;
      gui->fb_data.depth = 4;
    }

    reconstruct_texture = true;
  }

  if(reconstruct_texture) {
    gui->framebuffer = realloc(gui->framebuffer, w * h * gui->fb_data.depth);
    SDL_DestroyTexture(gui->texture);
    gui->texture = SDL_CreateTexture(gui->renderer, gui->fb_data.texFormat, SDL_TEXTUREACCESS_STREAMING, w, h);
  }

  if(format == f8888) {
    memcpy(gui->framebuffer, &gui->core.mem.rdram[origin & RDRAM_DSIZE], w * h * gui->fb_data.depth);
  } else {
    for(int i = 0; i < w * h * gui->fb_data.depth; i += gui->fb_data.depth) {
      gui->framebuffer[i] = gui->core.mem.rdram[HALF_ADDR(origin + i & RDRAM_DSIZE)];
      gui->framebuffer[i + 1] = gui->core.mem.rdram[HALF_ADDR(origin + 1 + i & RDRAM_DSIZE)];
    }
  }

  SDL_UpdateTexture(gui->texture, NULL, gui->framebuffer, w * gui->fb_data.depth);
}

void main_menubar(gui_t *gui, bool* running) {
  if(igBeginMainMenuBar()) {
    if(igBeginMenu("File", true)) {
      if(igMenuItem_Bool("Open", "O", false, true)) {
        open_file(gui);
      }
      if(igMenuItem_Bool("Exit", "Esc", false, true)) {
        *running = false;
      }
      igEndMenu();
    }
    if(igBeginMenu("Emulation", true)) {
      if(igMenuItem_Bool(gui->core.running ? "Pause" : "Resume", "P", false, gui->rom_loaded)) {
        gui->core.running = !gui->core.running;
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
  if(gui->debugger.count > 0) {
    ImVec2 window_size;
    igGetWindowSize(&window_size);
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

const char* regs_str[32] = {
  "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3",
  "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
  "t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"
};

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
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  igDestroyContext(gui->ctx);
  SDL_DestroyRenderer(gui->renderer);
  SDL_DestroyTexture(gui->texture);
  SDL_DestroyWindow(gui->window);
  SDL_Quit();
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
    pthread_create(&gui->emu_thread_id, NULL, core_cb, (void*)gui);
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