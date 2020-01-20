#pragma once
#include "../../common.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <filesystem>

struct GStateInfo {
    std::string name;
    int frames; // this information is used to get single frame image width from entire png
    std::string image;
    std::string shader;
};

// Helper class for parsing .gs config files
class GStatesFileReader {
public:
    static std::vector<GStateInfo> read_file(std::filesystem::path path);
};