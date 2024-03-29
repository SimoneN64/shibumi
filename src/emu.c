#include <emu.h>
#include <mem.h>
#include <access.h>
#include <audio.h>

void destroy_emu(emu_t* emu) {
  SDL_DestroyTexture(emu->texture);
  SDL_DestroyRenderer(emu->renderer);
  SDL_DestroyWindow(emu->window);
  SDL_Quit();
  NFD_Quit();
  destroy_core(&emu->core);
}

void init_emu(emu_t* emu) {
  init_core(&emu->core);
  emu->currentW = 320;
  emu->currentH = 240;
  emu->sdlFormat = SDL_PIXELFORMAT_RGBA5551;
  emu->currentFormat = f5553;

  SDL_Init(SDL_INIT_EVERYTHING);
  init_audio();
  emu->controller = NULL;
  emu->has_gamepad = false;

  emu->window = SDL_CreateWindow(
    "shibumi",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    W,
    H,
    SDL_WINDOW_RESIZABLE
  );
 
  emu->renderer = SDL_CreateRenderer(emu->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_RenderSetLogicalSize(emu->renderer, emu->currentW, emu->currentH);
  emu->texture = SDL_CreateTexture(
    emu->renderer,
    emu->sdlFormat,
    SDL_TEXTUREACCESS_STREAMING,
    emu->currentW,
    emu->currentH
  );

  emu->framebuffer = (u8*)calloc(emu->currentW * emu->currentH, 2);

  NFD_Init();
}

void emu_present(emu_t* emu) {
  mem_t* mem = &emu->core.mem;
  const u32 w = mem->mmio.vi.width, h = 0.75 * w;
  const u32 origin = mem->mmio.vi.origin & 0xFFFFFF;
  const u8 format = mem->mmio.vi.status.format;
  u8 depth = format == f8888 ? 4 : format == f5553 ? 2 : 0;
  bool reconstructTexture = false;

  const bool resChanged = emu->currentW != w || emu->currentH != h;
  const bool formatChanged = emu->currentFormat != format;

  if(resChanged) {
    emu->currentW = w;
    emu->currentH = h;

    reconstructTexture = true;
  }

  if(formatChanged) {
    emu->currentFormat = format;
    if(format == f5553) {
      emu->sdlFormat = SDL_PIXELFORMAT_RGBA5551;
      depth = 2;
    } else if(format == f8888) {
      emu->sdlFormat = SDL_PIXELFORMAT_RGBA8888;
      depth = 4;
    }

    reconstructTexture = true;
  }

  if(reconstructTexture) {
    emu->framebuffer = realloc(emu->framebuffer, w * h * depth);
    SDL_DestroyTexture(emu->texture);
    SDL_RenderSetLogicalSize(emu->renderer, (int)w, (int)h);
    emu->texture = SDL_CreateTexture(emu->renderer, emu->sdlFormat, SDL_TEXTUREACCESS_STREAMING, (int)w, (int)h);
  }

  if(format == f5553) {
    for(int i = 0; i < w * h * 2; i+=2) {
      u16 val = raccess_16(mem->rdram, (i + origin) & RDRAM_DSIZE);
      memcpy(&emu->framebuffer[i], &val, 2);
    }
  } else {
    for(int i = 0; i < w * h * 4; i+=4) {
      u32 val = raccess_32(mem->rdram, (i + origin) & RDRAM_DSIZE);
      memcpy(&emu->framebuffer[i], &val, 4);
    }
  }
  SDL_UpdateTexture(emu->texture, NULL, emu->framebuffer, (int) w * depth);
  SDL_RenderCopy(emu->renderer, emu->texture, NULL, NULL);
}

#define GET_BUTTON(gamepad, i) SDL_GameControllerGetButton(gamepad, i)
#define GET_AXIS(gamepad, axis) SDL_GameControllerGetAxis(gamepad, axis)

INLINE int clamp(int val, int min, int max) {
  if(val > max) {
    return max;
  } else if (val < min) {
    return min;
  }
  return val;
}

void poll_controller_gamepad(SDL_GameController* gamepad, controller_t* controller) {
  controller->b1 = 0;
  controller->b2 = 0;
  controller->b3 = 0;
  controller->b4 = 0;

  bool A = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_A);
  bool B = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_X);
  bool Z = GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) == 32767;
  bool START = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_START);
  bool DUP = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP);
  bool DDOWN = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  bool DLEFT = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  bool DRIGHT = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
  bool L = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
  bool R = GET_BUTTON(gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  bool CUP = GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_RIGHTY) == 32767;
  bool CDOWN = GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_RIGHTY) == -32768;
  bool CLEFT = GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_RIGHTX) == -32768;
  bool CRIGHT = GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_RIGHTX) == 32767;

  controller->b1 = (A << 7) | (B << 6) | (Z << 5) | (START << 4) |
                  (DUP << 3) | (DDOWN << 2) | (DLEFT << 1) | DRIGHT;

  controller->b2 = ((START && L && R) << 7) | (0 << 6) | (L << 5) | (R << 4) |
                  (CUP << 3) | (CDOWN << 2) | (CLEFT << 1) | CRIGHT;

  s8 xaxis = (s8)clamp((GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_LEFTX) >> 8), -127, 127);
  s8 yaxis = (s8)clamp(-(GET_AXIS(gamepad, SDL_CONTROLLER_AXIS_LEFTY) >> 8), -127, 127);

  controller->b3 = xaxis;
  controller->b4 = yaxis;

  if((controller->b2 >> 7) & 1) {
    controller->b1 &= ~0x10;
    controller->b3 = 0;
    controller->b4 = 0;
  }
}

void poll_controller_kbm(controller_t* controller, const u8* state) {
  controller->b1 = 0;
  controller->b2 = 0;
  controller->b3 = 0;
  controller->b4 = 0;
  controller->b1 =
    (state[SDL_SCANCODE_X] << 7) |
    (state[SDL_SCANCODE_C] << 6) |
    (state[SDL_SCANCODE_Z] << 5) |
    (state[SDL_SCANCODE_RETURN] << 4) |
    (state[SDL_SCANCODE_KP_8] << 3) |
    (state[SDL_SCANCODE_KP_5] << 2) |
    (state[SDL_SCANCODE_KP_4] << 1) |
    (state[SDL_SCANCODE_KP_6]);
  controller->b2 =
    ((state[SDL_SCANCODE_RETURN] && state[SDL_SCANCODE_A] && state[SDL_SCANCODE_S]) << 7) |
    (0 << 6) |
    (state[SDL_SCANCODE_A] << 5) |
    (state[SDL_SCANCODE_S] << 4) |
    (state[SDL_SCANCODE_I] << 3) |
    (state[SDL_SCANCODE_J] << 2) |
    (state[SDL_SCANCODE_K] << 1) |
    (state[SDL_SCANCODE_L]);

  s8 xaxis = state[SDL_SCANCODE_LEFT] ? -128 : (state[SDL_SCANCODE_RIGHT] ? 127 : 0);
  s8 yaxis = state[SDL_SCANCODE_DOWN] ? -128 : (state[SDL_SCANCODE_UP] ? 127 : 0);

  controller->b3 = xaxis;
  controller->b4 = yaxis;

  if((controller->b2 >> 7) & 1) {
    controller->b1 &= ~0x10;
    controller->b3 = 0;
    controller->b4 = 0;
  }
}

void emu_run(emu_t* emu) {
  core_t* core = &emu->core;
  controller_t* controller = &core->mem.mmio.si.controller;
  bool running = true;

  const u8* state = SDL_GetKeyboardState(NULL);

  while(running) {
    if(core->running) {
      run_frame(core);
      emu_present(emu);
    }

    SDL_RenderPresent(emu->renderer);

    if(emu->has_gamepad) {
      poll_controller_gamepad(emu->controller, controller);
    } else {
      poll_controller_kbm(controller, state);
    }

    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_CONTROLLERDEVICEADDED: {
          const int index = e.cdevice.which;
          emu->controller = SDL_GameControllerOpen(index);
          emu->has_gamepad = true;
        } break;
        case SDL_CONTROLLERDEVICEREMOVED:
          SDL_GameControllerClose(emu->controller);
          emu->has_gamepad = false;
          break;
        case SDL_QUIT:
          running = false;
          break;
        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_o: {
              nfdfilteritem_t filter = {"Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64"};
              nfdresult_t result = NFD_OpenDialog(&emu->romFile, &filter, 1, ".");
              if (result == NFD_OKAY) {
                core->running = false;
                init_core(core);
                core->running = load_rom(&core->mem, emu->romFile);
              }
            } break;
          } break;
      }
    }
  }
}
