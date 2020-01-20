
#include "texture_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// (Frees the image data)
bool TextureLoader::gl_load(unsigned char* data, GLuint& textureID, int width, int height, int color_channels) {
    using namespace std;
    auto gl_channel = GL_RGBA;
    if (color_channels == 3) {
        cout << "TEXTURELOAD::GL_LOAD: PROBABLY ERROR: IMAGE FILE HAS 3 CHANNELS, MAKING TEXTURE ANYWAY BUT IT WON'T HAVE ALPHA CHANNEL" << endl;
        cout << " (setting channel to RGB instead of RGBA)" << endl;
        gl_channel = GL_RGB;
    }

    gl_flush_errors();
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_channel, width, height, 0, gl_channel, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST = pixel perfect
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // (use GL_LINEAR if want linear filter instead)
    stbi_image_free(data);
    if (gl_has_errors()) {
        cout << "TEXTURELOADER::GL_LOAD ERROR: GL HAS ERRORS!" << endl;
    }
    return !gl_has_errors();
}

bool TextureLoader::load(std::string imageName, GLuint& textureID, int& width, int& height)
{
    using namespace std;
    string pathStr = textures_path_origin + imageName;
    return load_from_absolute_path(pathStr.c_str(), textureID, width, height);

}

bool TextureLoader::load_from_absolute_path(const char* path, GLuint& textureID, int& width, int& height)
{
    using namespace std;
    int color_channels;
    auto img_data = stbi_load(path, &width, &height, &color_channels, 0);
    if (img_data) {
        if (VERBOSE) {
            cout << "TextureLoader::load: STBI succesfully loaded " << path << endl;
            cout << " .. image width, height, color channels: " << width << " x " << height << " @" << color_channels << " channels";
        }
        if (gl_load(img_data, textureID, width, height, color_channels)) {
            if (VERBOSE) {
                cout << "TextureLoader::gl_load Successfully loaded texture: " << path << " into GLuint " << textureID << endl;
                cout << " .. image width, height: " << width << " x " << height << endl;
            }
            return true;
        }
        else {
            cout << "TEXTURELOADER::GL_LOAD FAILED FOR: " << path << ". Is it the right format?" << endl;
        }
    }
    else {
        cout << "TEXTURELOADER::LOAD: ERROR: STBI COULD NOT LOAD " << path << ". Does it exist and is it an image?" << endl;
        return false;
    }
    return false;
}
