#pragma once

#include "common.hpp"
#include <string>
#include <iostream>

class TextureLoader {
public:
    static bool load(std::string imageName, GLuint& textureID, int& width, int& height);
    static bool load_from_absolute_path(const char* path, GLuint& textureID, int& width, int& height);
private:
    static bool gl_load(unsigned char* data, GLuint& out_textureID, int width, int height, int color_channels);
};