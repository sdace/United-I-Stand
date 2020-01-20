#include "ECS/Systems/movement_system.hpp"

using namespace std;

void MovementSystem::init()
{
    EntityQuery query =
        EntityQuery().all(2,
                          ComponentType<PositionComponent>::get_id(),
                          ComponentType<VelocityComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void MovementSystem::update(float elapsed_sec)
{
	auto entities = m_entity_query->get_entities();

	for (const auto& e : entities)
	{
		PositionComponent pos = entity_manager->get_component_data<PositionComponent>(e);
		VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(e);
		pos.Value.x += vel.Value.x + vel.External.x;
		pos.Value.y += vel.Value.y + vel.External.y;
		vel.External = { 0.0f, 0.0f };

		entity_manager->set_component_data(e, move(pos));
		entity_manager->set_component_data(e, move(vel));
	}
}

void PreviousMovementSystem::init()
{
    EntityQuery query =
        EntityQuery().all(2,
                          ComponentType<PositionComponent>::get_id(),
                          ComponentType<VelocityComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void PreviousMovementSystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();

    for (const auto& e : entities)
    {
        VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(e);
        vel.Previous.x = vel.Value.x + vel.External.x;
        vel.Previous.y = vel.Value.y + vel.External.y;
        entity_manager->set_component_data(e, move(vel));
    }
}


