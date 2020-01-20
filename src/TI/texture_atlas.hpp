#pragma once

#include <fstream> // stdout, stderr..
#include <filesystem>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "common.hpp"
#include "project_path.hpp"
#include "G-Unit/util/texture_loader.hpp"

#define PATH_TA PROJECT_SOURCE_DIR "./assets/texture_atlas"
#define PATH_IMG PROJECT_SOURCE_DIR "./assets/texture_atlas/dungeon_tiles.png"

class TextureAtlas
{
public:

    static TextureAtlas& get_instance();

    // Shared between all tiles
    static GLuint tile_textures;

    // Returns texture coordinates within texture atlas of tile with desired index
    vec2* texCoord(int index);

    bool has_attribute(std::string attribute, int id);

    bool init();

    // Read information from tileset .tsx file to set values of private variables
    void Parse();

    // For debugging purposes only
    void debug();

private:

    // Total number of tiles within texture map
    int num_tiles;

    // Dimensions of tiles
    int tile_width;
    int tile_height;

    // Number of rows and columns of tiles in the texture atlas
    int tiles_x;
    int tiles_y;

    // Used for calculating texture coordinates within range [0,1]  e.g. tex_hstride = 1 / tiles_x.
    // Represents width and height of each tile in texture coordinates.
    float tex_hstride;
    float tex_vstride;

    std::vector<vec2*> tex_coords;

    std::unordered_map<int, std::string> attributes;

    void map_coordinates();
};