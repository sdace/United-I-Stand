#pragma once 

#include "common.hpp"
#include "spawners/spawner.hpp"
#include <vector>
#include <iostream>

// every info needed to spawn an enemy
struct MenuSpawnInfo
{
    vec2 position;
};

class MenuSpawner
{
public:
    void init(shared_ptr<EntityManager> entity_manager);
    void update(float elapsed_time);
};
