#pragma once

#include "ECS/entity_manager.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/tile_collider_component.hpp"
#include "ECS/Components/oscillator_component.hpp"

class OscillatingSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;
};
