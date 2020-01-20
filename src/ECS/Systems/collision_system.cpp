#include "ECS/Systems/collision_system.hpp"
#include "SoundPlayer/sound_player.hpp"

SoundPlayer& pmusic1 = SoundPlayer::get_instance();
using namespace std;

void CollisionSystem::init()
{
    EntityQuery collider_query =
        EntityQuery().all(4,
                          ComponentType<ColliderComponent>::get_id(),
                          ComponentType<AABBComponent>::get_id(),
                          ComponentType<PositionComponent>::get_id(),
                          ComponentType<VelocityComponent>::get_id());
    m_collider_query = make_shared<EntityQuery>(move(collider_query));
    add_query(m_collider_query);

    EntityQuery solid_query =
        EntityQuery().all(3,
                          ComponentType<SolidComponent>::get_id(),
                          ComponentType<AABBComponent>::get_id(),
                          ComponentType<PositionComponent>::get_id());
    m_solid_query = make_shared<EntityQuery>(move(solid_query));
    add_query(m_solid_query);

	EntityQuery enemies_query =
		EntityQuery().all(2,
			ComponentType<AABBComponent>::get_id(),
			ComponentType<EnemyComponent>::get_id());
	m_enemies_query = make_shared<EntityQuery>(move(enemies_query));
	add_query(m_enemies_query);

	EntityQuery player_query =
		EntityQuery().all(1,
			ComponentType<PlayerComponent>::get_id());
	m_player_query = make_shared<EntityQuery>(move(player_query));
	add_query(m_player_query);

	EntityQuery player_spears_query =
		EntityQuery().all(1,
			ComponentType<PlayerSpearComponent>::get_id());
	m_player_spears_query = make_shared<EntityQuery>(move(player_spears_query));
	add_query(m_player_spears_query);

	EntityQuery enemies_spears_query =
		EntityQuery().all(1,
			ComponentType<EnemySpearComponent>::get_id());
	m_enemies_spears_query = make_shared<EntityQuery>(move(enemies_spears_query));
	add_query(m_enemies_spears_query);

	pmusic1.initialize();
	pmusic1.loadsound("spear_hit.wav");

}

void CollisionSystem::update(float elapsed_sec)
{
	auto colliders = m_collider_query->get_entities();
	auto solids = m_solid_query->get_entities();
	auto enemies = m_enemies_query->get_entities();
	auto player = m_player_query->get_entities();
	auto player_spears = m_player_spears_query->get_entities();
	auto enemies_spears = m_enemies_spears_query->get_entities();

	for (const auto& collider : colliders)
	{
		AABBComponent collider_aabb = entity_manager->get_component_data<AABBComponent>(collider);

		// Set platform to none; we will update this with a platform if we hit one
		ColliderComponent collider_component = entity_manager->get_component_data<ColliderComponent>(collider);
		collider_component.Platform = Entity::NULL_ENTITY;
		entity_manager->set_component_data(collider, move(collider_component));

		for (const auto& solid : solids)
		{
			PositionComponent collider_position = entity_manager->get_component_data<PositionComponent>(collider);
			VelocityComponent collider_velocity = entity_manager->get_component_data<VelocityComponent>(collider);
			vec2 collider_center = { collider_position.Value.x + collider_aabb.Offset.x,
									 collider_position.Value.y + collider_aabb.Offset.y };
			vec2 projected_collider_position = { collider_center.x + collider_velocity.Value.x + collider_velocity.External.x,
												 collider_center.y + collider_velocity.Value.y + collider_velocity.External.y };

			AABBComponent solid_aabb = entity_manager->get_component_data<AABBComponent>(solid);
			PositionComponent solid_position = entity_manager->get_component_data<PositionComponent>(solid);
			vec2 solid_center = { solid_position.Value.x + solid_aabb.Offset.x,
								  solid_position.Value.y + solid_aabb.Offset.y };
			vec2 projected_solid_position;
			if (entity_manager->has_component<VelocityComponent>(solid))
			{
				VelocityComponent solid_velocity = entity_manager->get_component_data<VelocityComponent>(solid);
				projected_solid_position = { solid_position.Value.x + solid_velocity.Value.x + collider_velocity.External.x,
											 solid_position.Value.y + solid_velocity.Value.y + collider_velocity.External.y };
			}
			else
			{
				projected_solid_position = solid_position.Value;
			}

			if (Common::are_colliding(projected_collider_position.x, projected_collider_position.y, collider_aabb.HalfSize.x, collider_aabb.HalfSize.y,
				projected_solid_position.x, projected_solid_position.y, solid_aabb.HalfSize.x, solid_aabb.HalfSize.y))
			{
				collide(collider, solid, collider_center, solid_center, collider_aabb.HalfSize, solid_aabb.HalfSize);
			}
		}
	}

	if (player_spears.size() > 0) {
		auto it = player_spears.begin();
		auto player_spear = *it;
		PositionComponent spear_position = entity_manager->get_component_data<PositionComponent>(player_spear);
		for (const auto& enemy : enemies) {
			PositionComponent enemy_position = entity_manager->get_component_data<PositionComponent>(enemy);
			if (abs(spear_position.Value.x - enemy_position.Value.x) < 10 && abs(spear_position.Value.y - enemy_position.Value.y) < 10) {
				if (entity_manager->is_valid_entity(enemy)) {
					entity_manager->destroy_entity(enemy);
					pmusic1.playsound("spear_hit.wav");
				}
				if (entity_manager->is_valid_entity(player_spear)) {
					entity_manager->destroy_entity(player_spear);
				}
				break;
			}
		}
	}
	
	if (enemies_spears.size() > 0) {
		for (const auto& e : enemies_spears) {
            if (!entity_manager->is_valid_entity(e))
            {
                continue;
            }
			PositionComponent enemy_spear_position = entity_manager->get_component_data<PositionComponent>(e);
			if (player.size() > 0) {
				for (const auto& p : player) {
					PositionComponent player_position = entity_manager->get_component_data<PositionComponent>(p);
					if (abs(enemy_spear_position.Value.x - player_position.Value.x) < 10 && abs(enemy_spear_position.Value.y - player_position.Value.y) < 10) {
						destroy_all_enemies = true;
						event_manager->post(move(PlayerDeathEvent()));
						break;
					}
				}
			}
		}
	}
	if (enemies.size() > 0) {
		for (const auto& e : enemies) {
            if (!entity_manager->is_valid_entity(e))
            {
                continue;
            }
			PositionComponent enemy_position = entity_manager->get_component_data<PositionComponent>(e);
			if (player.size() > 0) {
				for (const auto& p : player) {
					PositionComponent player_position = entity_manager->get_component_data<PositionComponent>(p);
					if (abs(enemy_position.Value.x - player_position.Value.x) < 10 && abs(enemy_position.Value.y - player_position.Value.y) < 10) {
						destroy_all_enemies = true;
						event_manager->post(move(PlayerDeathEvent()));
						break;
					}
				}
			}
		}
	}

	if (destroy_all_enemies) {
		for (const auto& enemy : enemies) {
			if (entity_manager->is_valid_entity(enemy)) {
				entity_manager->destroy_entity(enemy);
			}
		}
		for (const auto& enemy_spear : enemies_spears) {
			if (entity_manager->is_valid_entity(enemy_spear)) {
				entity_manager->destroy_entity(enemy_spear);
			}
		}
		destroy_all_enemies = false;
	}
	
}

void CollisionSystem::collide(Entity collider, Entity solid,
                              vec2 collider_position, vec2 solid_position,
                              vec2 collider_halfsize, vec2 solid_halfsize)
{
    ColliderComponent collider_component = entity_manager->get_component_data<ColliderComponent>(collider);
    SolidComponent solid_component = entity_manager->get_component_data<SolidComponent>(solid);

    if (solid_component.SolidTop)
    {
        // If the collider was above the solid in the previous frame
        if (collider_position.y + collider_halfsize.y < solid_position.y - solid_halfsize.y + EPSILON)
        {
            collider_component.Platform = solid;

            VelocityComponent velocity = entity_manager->get_component_data<VelocityComponent>(collider);
            if (velocity.Value.y > 0)
            {
                velocity.Value.y = (solid_position.y - solid_halfsize.y) - (collider_position.y + collider_halfsize.y);
                velocity.External.y = 0;
                entity_manager->set_component_data(collider, move(velocity));
            }

            if (entity_manager->has_component<TileColliderComponent>(collider))
            {
                TileColliderComponent tile_collider = entity_manager->get_component_data<TileColliderComponent>(collider);
                tile_collider.Below = true;
                entity_manager->set_component_data(collider, move(tile_collider));
            }

            entity_manager->set_component_data(collider, move(collider_component));
        }
    }
}

