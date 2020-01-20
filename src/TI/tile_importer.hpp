#pragma once
// stlib
#include <fstream> // stdout, stderr..
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "TI/tilemap.hpp"
#include "project_path.hpp"
#define PATH_TILEMAPS PROJECT_SOURCE_DIR "./assets/tilemap_csv"

using namespace std;

class TileManager
{
public:
    static TileManager& get_instance();
    TileMap tile_map; // skeletal game only
    vector<TileMap> load_tilemaps();
    vector<vector<int>> parse_single_tilemap_csv(filesystem::path path);
};