#include <capstone.h>
#include <core.h>
#include <gui.h>
#include <utils.h>
#include <string>
#include <cstring>

INLINE void* core_callback(void* args) {
  Gui* gui = (Gui*)args;
  while(!atomic_load(&gui->emu_quit)) {
    clock_t begin = clock();
    run_frame(&gui->core);
    clock_t end = clock();
    gui->delta += end - begin;
  }
  
  return NULL;
}

Gui::Gui(const char* title) {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    logfatal("Error: %s\n", SDL_GetError());
  }

  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(0); // Enable vsync

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGuiStyle& style = ImGui::GetStyle();

  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  style.WindowRounding = 10;

  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  io.Fonts->AddFontFromFileTTF("resources/FiraCode-VariableFont_wght.ttf", 16);
  
  init_core(&core);
  init_disasm(&debugger);

  memory_editor.ReadOnly = true;
  memory_editor.ReadFn = read8_ignore_tlb_and_maps;
  memory_editor.Open = show_memory_editor;
  logger.InfoStr = message_type_strings[0].c_str();
  logger.WarnStr = message_type_strings[1].c_str();
  logger.ErrorStr = message_type_strings[2].c_str();

  framebuffer = (u8*)malloc(320 * 240 * 4);
  memset(framebuffer, 0x000000ff, 320 * 240 * 4);

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 240, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  pthread_create(&emu_thread, NULL, core_callback, (void*)this);

  NFD_Init();
}

ImVec2 image_size;

INLINE void resize_callback(ImGuiSizeCallbackData* data) {
  ImVec2 window_size = ImGui::GetWindowSize();
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

void Gui::MainLoop() {
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
  unsigned int frames = 0;
  while(running) {    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch(event.type) {
        case SDL_QUIT: running = false; break;
        case SDL_WINDOWEVENT:
          if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
            running = false;
          }
          break;  
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_o: OpenFile(); break;
            case SDLK_p:
              if(rom_loaded) {
                core.running = !core.running;
              }
              break;
            case SDLK_ESCAPE: running = false; break;
          }
          break;
      }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    if(show_metrics) ImGui::ShowMetricsWindow(&show_metrics);
    DebuggerWindow();
  
    ImGui::SetNextWindowSizeConstraints((ImVec2){0, 0}, (ImVec2){__FLT_MAX__, __FLT_MAX__}, resize_callback, NULL);
    ImGui::Begin("Display", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
    MainMenubar();
    UpdateTexture();
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 result = {static_cast<float>((window_size.x - image_size.x) * 0.5), static_cast<float>((window_size.y - image_size.y + 15) * 0.5)};
    ImGui::SetCursorPos(result);
    ImGui::Image((ImTextureID)((intptr_t)id), image_size, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
    ImGui::End();

    frames++;

    if(clock_to_ms(delta) > 1000.0) {
      fps = (double)frames * 0.5 + fps *0.5;
      frames = 0;
      delta -= CLOCKS_PER_SEC;
      frametime = 1000.0 / ((fps == 0) ? 0.001 : fps);
    }

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

    SDL_GL_SwapWindow(window);
  }
}

void Gui::UpdateTexture() {
  u32 w = core.mem.mmio.vi.width, h = 0.75 * w;
  u32 origin = core.mem.mmio.vi.origin & 0xFFFFFF;
  u8 format = core.mem.mmio.vi.status.format;
  bool reconstruct_texture = false;
  bool res_changed = gl_data.old_w != w || gl_data.old_h != h;
  bool format_changed = gl_data.old_format != format;

  if(res_changed) {
    gl_data.old_w = w;
    gl_data.old_h = h;

    reconstruct_texture = true;
  }

  if(format_changed) {
    gl_data.old_format = format;
    if(format == f5553) {
      gl_data.glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
      gl_data.depth = 2;
    } else if (format == f8888) {
      gl_data.glFormat = GL_UNSIGNED_INT_8_8_8_8;
      gl_data.depth = 4;
    }

    reconstruct_texture = true;
  }

  if(reconstruct_texture) {
    framebuffer = (u8*)realloc(framebuffer, w * h * gl_data.depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, gl_data.glFormat, framebuffer);
  }

  memory_regions_t* memory_regions = &core.mem.memory_regions;

  if(format == f8888) {
    framebuffer[4] = 0xff;
    memcpy(framebuffer, &memory_regions->rdram[origin & RDRAM_DSIZE], w * h * gl_data.depth);
    for(int i = 0; i < w * h * gl_data.depth; i += gl_data.depth) {
      framebuffer[i + 4] |= 0xff;
    }
  } else {
    framebuffer[1] |= 1;
    for(int i = 0; i < w * h * gl_data.depth; i += gl_data.depth) {
      framebuffer[i] = memory_regions->rdram[HALF_ADDR(origin + i & RDRAM_DSIZE)];
      framebuffer[i + 1] = memory_regions->rdram[HALF_ADDR(origin + 1 + i & RDRAM_DSIZE)] | (1 << 16);
    }
  }

  glBindTexture(GL_TEXTURE_2D, id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, gl_data.glFormat, framebuffer);
}

void Gui::MainMenubar() {
  ImVec2 window_size = ImGui::GetWindowSize();
  if(ImGui::BeginMenuBar()) {
    if(ImGui::BeginMenu("File")) {
      if(ImGui::MenuItem("Open", "O")) {
        OpenFile();
      }

      if(ImGui::MenuItem("Exit", "Esc")) {
        running = false;
      }
      ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Emulation")) {
      std::string pause_text = "Pause";
      if(!core.running && rom_loaded) {
        pause_text = "Resume";
      }

      if(ImGui::MenuItem(pause_text.c_str(), "P", false, rom_loaded)) {
        core.running = !core.running;
        if(core.running) {
          core.stepping = false;
        }
      }

      if(ImGui::MenuItem("Stop", NULL, false, rom_loaded)) {
        Stop();
      }

      if(ImGui::MenuItem("Reset", NULL, false, rom_loaded)) {
        Reset();
      }

      ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Settings")) {
      ImGui::MenuItem("Show disassembly", NULL, &show_disasm, true);
      ImGui::MenuItem("Show register watch", NULL, &show_regs, true);
      ImGui::MenuItem("Show logs", NULL, &show_logs, true);
      if(ImGui::MenuItem("Show memory editor", NULL, &show_memory_editor, true)) {
        memory_editor.Open = show_memory_editor;
      }
      ImGui::MenuItem("Show metrics", NULL, &show_metrics, true);
      ImGui::EndMenu();
    }
    ImVec2 close_button_size = ImGui::CalcTextSize("[X]");
    char fps_text[255];
    sprintf(fps_text, rom_loaded ? "[ %.2f fps ][ %.2f ms ]" : "[ NaN fps ][ NaN ms ]", fps, frametime);
    ImVec2 fps_size = ImGui::CalcTextSize(fps_text);
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SameLine(window_size.x - close_button_size.x - fps_size.x - style.ItemInnerSpacing.x * 4 - 12, -1);
    ImGui::Text("%s", fps_text);
    ImGui::SameLine(window_size.x - close_button_size.x - style.ItemInnerSpacing.x * 2 - 12, -1);
    if(ImGui::BeginMenu("[X]")) {
      running = false;
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }
}

void Gui::Disassembly() {
  u32 instructions[25] = {};
  u32 pc = core.cpu.regs.pc;
  u32 pointer = pc - (14 * 4);
  pointer -= (pointer & 3); // align the pointer

  if(rom_loaded) {
    for(int i = 0; i < 25; i++) {
      instructions[i] = read32(&core.mem, pointer + i * 4);
    }
  } else {
    memset(instructions, 0xFFFFFFFF, 100);
  }

  u8 code[100];
  memcpy(code, instructions, 100);
  
  debugger.count = cs_disasm(debugger.handle, code, sizeof(code), pointer, 25, &debugger.insn);
  ImGui::Begin("Disassembly", &show_disasm, 0);

  ImVec2 window_size = ImGui::GetWindowSize();

  if(ImGui::Button("Step", (ImVec2){ (window_size.x / 2) - 10, 20 })) {
    core.stepping = true;
    step(&core.cpu, &core.mem);
  }

  ImGui::SameLine(window_size.x / 2, 5);
  if(ImGui::Button("Run frame", (ImVec2){ (window_size.x / 2) - 10, 20 })) {
    core.stepping = true;
    for(int i = 0; i < 100000; i++) {
      step(&core.cpu, &core.mem);
    }
  }

  static int num_instr = 0;

  char run_n_instr_str[20];
  sprintf(run_n_instr_str, "Run %d instr", num_instr);
  run_n_instr_str[19] = '\0';

  if(ImGui::Button(run_n_instr_str, (ImVec2){ (window_size.x / 3) - 10, 20 })) {
    core.stepping = true;
    for(int i = 0; i < num_instr; i++) {
      step(&core.cpu, &core.mem);
    }
  }
  
  ImGui::SameLine(window_size.x / 3, 5);
  ImGui::SetNextItemWidth(window_size.x / 10);
  ImGui::InputInt("Instruction count to run", &num_instr, 0, 0, ImGuiInputTextFlags_CharsNoBlank);
  
  static int addr = 0xFFFFFFFF;

  ImGui::InputInt("Address", &addr, 0, 0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank);

  ImGui::SameLine(window_size.x - (window_size.x / 4), 5);

  if(ImGui::Button("Set breakpoint", (ImVec2){(window_size.x / 4) - 10, 20})) {
    core.break_addr = addr;
  }

  ImGui::Spacing();

  ImGui::BeginChild("frame");
  if(debugger.count > 0) {
    for(size_t j = 0; j < debugger.count; j++) {
      std::string op_str = debugger.insn[j].op_str;
      const float font_size = ImGui::GetFontSize() * op_str.length() / 2;
      switch(j) {
      case 12 ... 14:
        ImGui::TextColored(colors_disasm[j & 3], "0x%" PRIx64 ":\t%s", debugger.insn[j].address, debugger.insn[j].mnemonic);
        ImGui::SameLine(window_size.x - font_size - 10, -1);
        ImGui::TextColored(colors_disasm[j & 3], "%s", debugger.insn[j].op_str);
        break;
      default:
        ImGui::Text("0x%" PRIx64 ":\t%s", debugger.insn[j].address, debugger.insn[j].mnemonic);
        ImGui::SameLine(window_size.x - font_size - 10, -1);
        ImGui::Text("%s", debugger.insn[j].op_str);
      }
    }

    cs_free(debugger.insn, debugger.count);
  } else {
    ImGui::Text(rom_loaded ? "Could not disassemble instruction!" : "No game loaded!");
  }
  ImGui::EndChild();
  ImGui::End();
}

void Gui::LogWindow() {
  std::string final_message{};
  if(last_message != nullptr && strcmp(last_message, "") && strcmp(last_message, old_message.c_str())) {
    if(last_message_type == FATAL) {
      Stop();
    }
    old_message = std::string(last_message);
    old_message_type = last_message_type;
    final_message = message_type_strings[last_message_type] + " " + old_message;
    logger.AddLog("%s", final_message.c_str());
  }
  logger.Draw("Logs", &show_logs);
}

void Gui::RegistersView() {
  registers_t* regs = &core.cpu.regs;
  ImGui::Begin("Registers view", &show_regs, 0);
  for(int i = 0; i < 32; i+=4) {
    ImGui::Text("%4s: %08lX %4s: %08lX %4s: %08lX %4s: %08lX", regs_str[i], regs->gpr[i], regs_str[i + 1], regs->gpr[i + 1], regs_str[i + 2], regs->gpr[i + 2], regs_str[i + 3], regs->gpr[i + 3]);
  }
  ImGui::Separator();
  s64 pipe[3] = {regs->old_pc, regs->pc, regs->next_pc};
  for(int i = 0; i < 3; i++) {
    ImGui::Text("pipe[%d]: %08lX", i, pipe[i]);
  }
  ImGui::End();
}

void Gui::DebuggerWindow() {
  if(show_disasm) Disassembly();
  show_memory_editor = memory_editor.Open;
  if(show_memory_editor) memory_editor.DrawWindow("Memory editor", &core.mem, 0xFFFFFFFF);
  if(show_regs) RegistersView();
  if(show_logs) LogWindow();
}

Gui::~Gui() {
  emu_quit = true;
  pthread_join(emu_thread, NULL);
  destroy_disasm(&debugger);
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Gui::OpenFile() {
  nfdfilteritem_t filter = { "Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64" };
  nfdresult_t result = NFD_OpenDialog(&rom_file, &filter, 1, EMU_DIR);
  if(result == NFD_OKAY) {
    Reset();
  }
}

void Gui::Start() {
  rom_loaded = load_rom(&core.mem, rom_file);
  emu_quit = !rom_loaded;
  core.running = rom_loaded;
  if(rom_loaded) {
    pthread_create(&emu_thread, NULL, core_callback, (void*)this);
  }
}

void Gui::Reset() {
  Stop();
  Start();
}

void Gui::Stop() {
  emu_quit = true;
  pthread_join(emu_thread, NULL);
  init_core(&core);
  rom_loaded = false;
  core.running = false;
}