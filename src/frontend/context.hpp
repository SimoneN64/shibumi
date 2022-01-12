#pragma once
#include <gui.hpp>
#include <core.h>

namespace Shibumi
{
struct Emulator;
struct Context {
  Context();
  uint id;
  void UpdateTexture(Emulator& emu, core_t& core);
  u32 old_w = 320, old_h = 240;
  u8 old_format = 14;
  int glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
  u8 depth = 2;
  u8* framebuffer;
};
}