#pragma once

#include "common.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/checkpoint_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/entity_manager.hpp"
#include "Events/activate_checkpoint_event.hpp"
#include "G-Unit/g_unit.hpp"
#include "event_manager.hpp"

#include <iostream>

class CheckpointSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    void update_checkpoints();
    void animate_checkpoints();

    shared_ptr<EntityQuery> m_checkpoint_query;
    shared_ptr<EntityQuery> m_player_query;

    int m_frame_duration;
    int m_frame_timer;
};
