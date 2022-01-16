#pragma once
#include <string>
#include <common.h>
#include <glad.h>

unsigned int CreateIDFromImage(const std::string& path);
unsigned int CreateIDFromBuffer(const u8* buffer, int w, int h, GLenum internalFormat = GL_RGBA, GLenum textureFormat = GL_UNSIGNED_INT_8_8_8_8);