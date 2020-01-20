#pragma once

#include "ECS/Components/collider_component.hpp"
#include "ECS/Components/solid_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/tile_collider_component.hpp"
#include "ECS/Components/enemy_component.hpp"
#include "ECS/Components/enemy_spear_component.hpp"
#include "ECS/Components/player_spear_component.hpp"

#include "ECS/Components/killable_component.hpp"
#include "ECS/Components/hurtbox_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/gravity_component.hpp"
#include "ECS/Components/enemy_component.hpp"
#include "ECS/entity_manager.hpp"
#include "common.hpp"
#include "stdlib.h"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/death_component.hpp"
#include "checkpoint_service.hpp"
#include <ECS/Systems/player_death_system.hpp>


class CollisionSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    void collide(Entity collider, Entity solid,
                 vec2 collider_position, vec2 solid_position,
                 vec2 collider_halfsize, vec2 solid_halfsize);
    shared_ptr<EntityQuery> m_collider_query;
    shared_ptr<EntityQuery> m_solid_query;
    shared_ptr<EntityQuery> m_enemies_query;
    shared_ptr<EntityQuery> m_player_query;
    shared_ptr<EntityQuery> m_player_spears_query;
    shared_ptr<EntityQuery> m_enemies_spears_query;
	bool destroy_all_enemies = false;
};
