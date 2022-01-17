#include <core.h>
#include <context.hpp>
#include <nfd.hpp>

int main(int argc, char* argv[]) {
  core_t core;
  init_core(&core);

  if(argc > 1) {
    core.running = load_rom(&core.mem, argv[1]);
  }

  Context context;
  NFD::Init();
  nfdchar_t* romFile;

  bool running = true;

  while(running) {
    if(core.running) {
      run_frame(&core);
      context.Present(core.mem);
      SDL_RenderCopy(context.renderer, context.texture, nullptr, nullptr);
      SDL_RenderPresent(context.renderer);
    } else {
      SDL_SetRenderDrawColor(context.renderer, 0, 0, 0, 0);
      SDL_RenderClear(context.renderer);
      SDL_RenderPresent(context.renderer);
    }

    SDL_Event e;
    SDL_PollEvent(&e);
    switch(e.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
          case SDLK_o: {
            nfdfilteritem_t filter = { "Nintendo 64 roms", "n64,z64,v64,N64,Z64,V64" };
            nfdresult_t result = NFD_OpenDialog(&romFile, &filter, 1, ".");
            if(result == NFD_OKAY) {
              core.running = false;
              init_core(&core);
              core.running = load_rom(&core.mem, romFile);
            }
          } break;
        }
        break;
    }
  }

  NFD::Quit();

  return 0;
}