#pragma once 

#include "common.hpp"
#include "spawners/spawner.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "TI/room_service.hpp"
#include <vector>
#include <iostream>

// every info needed to spawn an enemy
struct PlatformSpawnInfo
{
    vec2 position;
    bool is_circular;
    vec2 displacement;
    float speed;
    string type;
};

class PlatformSpawner : public Spawner
{
public:
    // read the tmx file once, and save all enemy information by room
    void init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager) override;
    void update(float elapsed_time) override;
    void make_platform(std::shared_ptr<EntityManager>, PlatformSpawnInfo p);

private:
    pugi::xml_document doc;
};