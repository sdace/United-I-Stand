#pragma once

#include "common.hpp"
#include "ECS/entity_manager.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/upgrade_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "Events/upgrade_unlocked_event.hpp"

class UpgradeSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_player_query;
    shared_ptr<EntityQuery> m_upgrade_query;
};
