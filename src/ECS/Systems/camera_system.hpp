#pragma once

#include "ECS/Components/player_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/entity_manager.hpp"
#include "common.hpp"
#include "../../G-Unit/g_unit.hpp"

#include <iostream>

class CameraSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_player_query;
};
