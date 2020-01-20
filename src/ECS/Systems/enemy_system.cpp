#include "ECS/Systems/enemy_system.hpp"

using namespace std;
#define SPEAR_SPEED 2.f
void EnemySystem::init()
{
	EntityQuery player_query = EntityQuery().all(1, ComponentType<PlayerComponent>::get_id());
	EntityQuery enemies_query = EntityQuery().all(1, ComponentType<EnemyComponent>::get_id());
	m_player_query = make_shared<EntityQuery>(move(player_query));
	m_enemies_query = make_shared<EntityQuery>(move(enemies_query));
	entity_manager->register_query(m_player_query);
	entity_manager->register_query(m_enemies_query);
}

void EnemySystem::update(float elapsed_sec)
{
	
	unordered_set<Entity> player_entities = m_player_query->get_entities();
	for (const Entity& e : player_entities) {
		PositionComponent player_position_component = entity_manager->get_component_data<PositionComponent>(e);
		unordered_set<Entity> enemies_entities = m_enemies_query->get_entities();
		for (const Entity& enemy : enemies_entities) {
			EnemyComponent enemy_component = entity_manager->get_component_data<EnemyComponent>(enemy);
			int animation_frame = enemy_component.animation_frame;
			if (animation_frame == 0) {
				RenderableComponent enemy_renderable_component = entity_manager->get_component_data<RenderableComponent>(enemy);
				if (enemy_renderable_component.gstate == "slime") {
					GUnit::get_instance().advance_frame("slime");
				}
				if (enemy_renderable_component.gstate == "bird") {
					GUnit::get_instance().advance_frame("bird");
				}
				enemy_component.animation_frame = 30;
				entity_manager->set_component_data(enemy, move(enemy_renderable_component));
			}
			else {
				enemy_component.animation_frame--;
			}
			entity_manager->set_component_data(enemy, move(enemy_component));
			if (enemy_component.Trace_Player) {
				enemy_trace_player(enemy, player_position_component, enemy_component.Trace_Speed);
			}
			if (enemy_component.Shoot_Spear) {
				enemy_throw_spear(enemy);
			}
		}
	}
	
}

void EnemySystem::enemy_trace_player(Entity enemy, PositionComponent player_position_component, float enemy_trace_speed)
{
	PositionComponent enemy_position_component = entity_manager->get_component_data<PositionComponent>(enemy);
	VelocityComponent enemy_velocity_component = entity_manager->get_component_data<VelocityComponent>(enemy);
	FacingComponent enemy_facing_component = entity_manager->get_component_data<FacingComponent>(enemy);
	vec2 player_position = player_position_component.Value;
	vec2 enemy_position = enemy_position_component.Value;
	float enemy_tracing_player_speed = enemy_trace_speed;
	float stop_tracing_distance = enemy_trace_speed / 2;
	if (should_enemy_trace_player(player_position, enemy_position)) {
		vec2 dif = { player_position.x - enemy_position.x, player_position.y - enemy_position.y };
		if (dif.x > stop_tracing_distance) {
			enemy_velocity_component.Value.x += enemy_tracing_player_speed;
			enemy_facing_component.Value = RIGHT;
		}
		else{
			enemy_velocity_component.Value.x -= enemy_tracing_player_speed;
			enemy_facing_component.Value = LEFT;
		}
		entity_manager->set_component_data(enemy, move(enemy_velocity_component));
		entity_manager->set_component_data(enemy, move(enemy_facing_component));
		enemy_jump(enemy);
	}
}

bool EnemySystem::should_enemy_trace_player(vec2 player_position, vec2 enemy_position) {
	return (enemy_position.x > player_position.x - SCREEN_WIDTH / 2 || enemy_position.x < player_position.x + SCREEN_WIDTH / 2)
		&& (enemy_position.y > player_position.y - SCREEN_HEIGHT / 4 || enemy_position.y < player_position.y + SCREEN_HEIGHT / 4);
}

void EnemySystem::enemy_jump(Entity enemy) {
	EnemyComponent enemy_component = entity_manager->get_component_data<EnemyComponent>(enemy);
	VelocityComponent enemy_velocity_component = entity_manager->get_component_data<VelocityComponent>(enemy);
	// setting how many frames for one jump
	if (enemy_component.jump_frame == 0.f) {
		enemy_component.jump_frame = 120.f;
	}
	else if (enemy_component.jump_frame == 1.f) {
		// setting how many frames one jump will finish, 60 here means 30
		float jump_finished_frame = 60.f;
		if (enemy_component.jump_finished_frame == -1.f) {
			enemy_component.jump_finished_frame = jump_finished_frame;
		}
		// jump_up_speed is for how fast the enemy can jump
		float jump_up_speed = 2.5f;
		// falling_speed is for how fash the enemy can fall after finish jumping
		float falling_speed = 3.f;

		if (enemy_component.is_jump_up == true) {
			//enemy_position_component.Value.y -= jump_up_speed;
			enemy_velocity_component.Value.y -= jump_up_speed;
			if (enemy_component.jump_finished_frame != 0.f) {
				enemy_component.jump_finished_frame -= jump_up_speed;
			}
			else {
				enemy_component.is_jump_up = false;
			}
		}
		else {
			//	enemy_position_component.Value.y += falling_speed;
			if (enemy_component.jump_finished_frame != jump_finished_frame) {
				enemy_component.jump_finished_frame += falling_speed;
			}
			else {
				enemy_component.is_jump_up = true;
				enemy_component.jump_frame = 0.f;
			}
		}
	}
	else {
		enemy_component.jump_frame--;
	}
	entity_manager->set_component_data(enemy, move(enemy_velocity_component));
	entity_manager->set_component_data(enemy, move(enemy_component));
}

void EnemySystem::enemy_throw_spear(Entity enemy)
{
	EnemyComponent enemy_component = entity_manager->get_component_data<EnemyComponent>(enemy);
	int frame = enemy_component.throw_spear_frame;
	if (frame % 120 == 0) {
		/*
		if (entity_manager->is_valid_entity(enemy_component.enemy_spear))
		{
			entity_manager->destroy_entity(enemy_component.enemy_spear);
		}
		*/
		Entity spear = entity_manager->create_entity();
		PositionComponent pos = entity_manager->get_component_data<PositionComponent>(enemy);
		pos.Value.y -= 2;
		FacingComponent facing = entity_manager->get_component_data<FacingComponent>(enemy);
		VelocityComponent vel = VelocityComponent({ facing.Value * SPEAR_SPEED, 0.f }, { facing.Value * SPEAR_SPEED, 0.f });
		AABBComponent aabb = AABBComponent({ 0.f, 3.f }, { 1.f, 1.f });
		TileColliderComponent tile_collider{};
		RenderableComponent renderable = RenderableComponent("bullet");
		EnemySpearComponent enemySpear = EnemySpearComponent({ 16.f,3.f });
	
		entity_manager->add_component_data(spear, move(facing));
		entity_manager->add_component_data(spear, move(pos));
		entity_manager->add_component_data(spear, move(vel));
		entity_manager->add_component_data(spear, move(aabb));
		entity_manager->add_component_data(spear, move(tile_collider));
		entity_manager->add_component_data(spear, move(renderable));
		entity_manager->add_component_data(spear, move(enemySpear));

		enemy_component.enemy_spear = spear;
	}
	enemy_component.throw_spear_frame++;
	entity_manager->set_component_data(enemy, move(enemy_component));
}

