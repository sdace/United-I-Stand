#include "ECS/Systems/collision_movement_system.hpp"

using namespace std;

void CollisionMovementSystem::init()
{
    EntityQuery query =
        EntityQuery().all(2,
                          ComponentType<ColliderComponent>::get_id(),
                          ComponentType<VelocityComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void CollisionMovementSystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();

    for (const auto& e : entities)
    {
        ColliderComponent collider = entity_manager->get_component_data<ColliderComponent>(e);

        if (entity_manager->is_valid_entity(collider.Platform))
        {
            if (entity_manager->has_component<VelocityComponent>(collider.Platform))
            {
                VelocityComponent platform_velocity = entity_manager->get_component_data<VelocityComponent>(collider.Platform);
                VelocityComponent collider_velocity = entity_manager->get_component_data<VelocityComponent>(e);

                collider_velocity.External.x += platform_velocity.Value.x;
                collider_velocity.External.y += platform_velocity.Value.y;

                entity_manager->set_component_data(e, move(collider_velocity));
            }
        }
    }
}