#pragma once
#include <glad.h>
#include <string>
#include <common.h>
#include <stb_image.h>

constexpr static int internalFormats[4] = { GL_R8, GL_RG, GL_RGB, GL_RGBA };
constexpr static int textureFormats[4] = { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_UNSIGNED_INT_8_8_8_8 };

unsigned int CreateIDFromImage(const std::string& path) {
  int w, h, n;
  u8* buf = stbi_load("resources/arrowDown.png", &w, &h, &n, 0);
  unsigned int id;
  if(buf) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormats[n], w, h, 0, internalFormats[n], textureFormats[n], buf);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  }

  stbi_image_free(buf);
}