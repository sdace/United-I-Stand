#pragma once

#include "common.hpp"
#include "ECS/entity_manager.hpp"
#include "Events/player_death_event.hpp"
#include "Events/player_respawn_event.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/death_component.hpp"
#include "ECS/Components/enemy_component.hpp"
#include "ECS/Components/enemy_spear_component.hpp"
#include "checkpoint_service.hpp"
#include "spawners/enemy_spawner.hpp"
#include "spawners/platform_spawner.hpp"

class PlayerDeathSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    void player_death_handler(PlayerDeathEvent ev);

    shared_ptr<EntityQuery> m_query;
    shared_ptr<EntityQuery> m_enemies_query;
};
