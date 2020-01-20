#pragma once

#include "common.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "TI/room_service.hpp"

#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <sstream>
#include <map>

class PlayerSpawnPositionService {
public:
    static PlayerSpawnPositionService& get_instance();
    PlayerSpawnPositionService(PlayerSpawnPositionService const&) = delete;
    void operator= (PlayerSpawnPositionService const&) = delete;
    vec2 position(int room);
    void init();
private:
    PlayerSpawnPositionService();
    std::map<int, vec2> m_spawn_positions;
    pugi::xml_document doc;
};