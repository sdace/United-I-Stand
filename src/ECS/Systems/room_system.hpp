#pragma once

#include "common.hpp"
#include "ECS/entity_manager.hpp"
#include "Events/player_death_event.hpp"
#include "Events/player_respawn_event.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/death_component.hpp"
#include "ECS/Components/enemy_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "Events/player_death_event.hpp"
#include "Events/room_change_event.hpp"
#include "TI/room_service.hpp"
#include "scene_service.hpp"

class RoomSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_query;
	shared_ptr<EntityQuery> m_enemies_query;
};
