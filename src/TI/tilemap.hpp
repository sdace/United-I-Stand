#pragma once

#include <vector>
#include <string>
#include <map>
#include "common.hpp"
class TileMap
{
public:
    vec3 colorFromWorldCoords(int x, int y)
    {
        return color(x / TILE_SIZE, y / TILE_SIZE);
    }

    // Returns the color of tile (x, y)
    // for skeletal game only
    vec3 color(int x, int y);

    // Returns whether the tile (x, y) has the given attribute
    bool is_attribute(std::string attribute, int x, int y);

    // 2D vector of tile IDs representing tiles (tiles[0][0] is top left corner)
    std::vector<std::vector<int>> tiles;
    // Filename of TileMap (including .csv extension)
    std::string filename;
    // a map of tile id to color
    std::map<int, vec3> skeletal_tile;

    // Number of rows in TileMap
    int width = -1; // set by tiles::load_tilemaps
    // Number of cols in TileMap
    int height = -1; // set by tiles::load_tilemaps
private:
    /**
    2d vector of ints, each int represents a tile ID
    -1 means the tile was not painted on in Tiled
    */
};