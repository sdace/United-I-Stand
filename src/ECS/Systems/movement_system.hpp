#pragma once

#include "ECS/Components/killable_component.hpp"
#include "ECS/Components/renderable_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/player_input_component.hpp"
#include "ECS/Components/gravity_component.hpp"
#include "ECS/Components/aabb_component.hpp"
#include "ECS/Components/tile_collider_component.hpp"
#include "ECS/Components/solid_component.hpp"
#include "ECS/Components/facing_component.hpp"
#include "ECS/Components/hurtbox_component.hpp"
#include "ECS/Components/death_component.hpp"
#include "ECS/Components/enemy_component.hpp"
#include "ECS/Components/oscillator_component.hpp"
#include "ECS/Components/enemy_spear_component.hpp"
#include "ECS/entity_manager.hpp"
#include "G-Unit/g_unit.hpp"

class MovementSystem : public EntitySystem
{
public:
    void init() override;
	void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;
};

class PreviousMovementSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;
};