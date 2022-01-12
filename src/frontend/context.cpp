#include <context.hpp>
#include <frontend.hpp>

namespace Shibumi
{
Context::Context() {
  framebuffer = (u8*)malloc(320 * 240 * 4);
  memset(framebuffer, 0, 320 * 240 * 4);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 240, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void Context::UpdateTexture(Emulator& emu, core_t& core) {
  // emu.emuMutex.lock();
  
  u32 w = core.mem.mmio.vi.width, h = 0.75 * w;
  u32 origin = core.mem.mmio.vi.origin & 0xFFFFFF;
  u8 format = core.mem.mmio.vi.status.format;

  // emu.emuMutex.unlock();
  
  bool reconstruct_texture = false;
  bool res_changed = old_w != w || old_h != h;
  bool format_changed = old_format != format;

  if(res_changed) {
    old_w = w;
    old_h = h;

    reconstruct_texture = true;
  }

  if(format_changed) {
    old_format = format;
    if(format == f5553) {
      glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
      depth = 2;
    } else if (format == f8888) {
      glFormat = GL_UNSIGNED_INT_8_8_8_8;
      depth = 4;
    }

    reconstruct_texture = true;
  }

  if(reconstruct_texture) {
    framebuffer = (u8*)realloc(framebuffer, w * h * depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, glFormat, framebuffer);
  }

  // emu.emuMutex.lock();

  if(format == f8888) {
    framebuffer[4] = 0xff;
    memcpy(framebuffer, &core.mem.rdram[origin & RDRAM_DSIZE], w * h * depth);
    for(int i = 0; i < w * h * depth; i += depth) {
      framebuffer[i + 4] |= 0xff;
    }
  } else {
    framebuffer[1] |= 1;
    for(int i = 0; i < w * h * depth; i += depth) {
      framebuffer[i] = core.mem.rdram[HALF_ADDR(origin + i & RDRAM_DSIZE)];
      framebuffer[i + 1] = core.mem.rdram[HALF_ADDR(origin + 1 + i & RDRAM_DSIZE)] | (1 << 16);
    }
  }

  // emu.emuMutex.unlock();

  glBindTexture(GL_TEXTURE_2D, id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, glFormat, framebuffer);
}
}