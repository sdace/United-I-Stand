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



class EnemySystem : public EntitySystem
{
public:
	void init() override;
	void update(float elapsed_sec) override;

private:
	shared_ptr<EntityQuery> m_enemies_query;
	shared_ptr<EntityQuery> m_player_query;
	void enemy_trace_player(Entity enemy, PositionComponent player_position_component, float enemy_trace_speed);
	void enemy_jump(Entity enemy);
	bool should_enemy_trace_player(vec2 player_position, vec2 enemy_position);
	void enemy_throw_spear(Entity enemy);
};
