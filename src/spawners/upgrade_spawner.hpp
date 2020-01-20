#pragma once 

#include "common.hpp"
#include "spawners/spawner.hpp"
#include "TI/tmx_parser/pugixml.hpp"
#include "ECS/Components/upgrade_component.hpp"
#include <vector>
#include <iostream>

class UpgradeSpawner : public Spawner
{
public:
    // read the tmx file once, and save all enemy information by room
    void init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager) override;
    void update(float elapsed_time) override;
    void make_upgrade(std::shared_ptr<EntityManager>, UpgradeType p, float x, float y);

private:
    pugi::xml_document doc;
};