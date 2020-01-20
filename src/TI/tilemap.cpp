#include "tilemap.hpp"
#include "texture_atlas.hpp"

vec3 TileMap::color(int x, int y) {
    return skeletal_tile[tiles[x][y]];
}

bool TileMap::is_attribute(std::string attribute, int x, int y) {
    if (x < 0 || x >= height || y < 0 || y >= width)
    {
        return true;
    }

    auto atlas = TextureAtlas::get_instance();
    int id = tiles[x][y];

    if (attribute == "solid")
    {
        return id != -1 && !atlas.has_attribute("background", id);
    }

    return atlas.has_attribute(attribute, id);
}
