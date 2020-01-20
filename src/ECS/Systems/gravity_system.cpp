#include "ECS/Systems/gravity_system.hpp"

using namespace std;

void GravitySystem::init()
{
    EntityQuery query =
        EntityQuery().all(2,
                          ComponentType<GravityComponent>::get_id(),
                          ComponentType<VelocityComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void GravitySystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();

    for (const auto& e : entities)
    {
        VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(e);
        GravityComponent grav = entity_manager->get_component_data<GravityComponent>(e);

        vel.Value.y += GRAVITY*grav.weight;
        vel.clamp();

        entity_manager->set_component_data(e, move(vel));
    }
}
