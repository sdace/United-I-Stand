#include "ECS/Systems/oscillating_system.hpp"

void OscillatingSystem::init()
{
    EntityQuery query =
        EntityQuery().all(1,
            ComponentType<OscillatorComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void OscillatingSystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();

    for (const auto& e : entities)
    {
        PositionComponent pos = entity_manager->get_component_data<PositionComponent>(e);
        VelocityComponent vel = entity_manager->get_component_data<VelocityComponent>(e);
        OscillatorComponent osc = entity_manager->get_component_data<OscillatorComponent>(e);

        osc.Counter += 0.01f * (float) osc.Speed;

        float simulated_displacement_x = sin(osc.Counter) * osc.Displacement.x;
        float simulated_displacement_y;
        if (osc.Circular)
        {
            simulated_displacement_y = cos(osc.Counter) * osc.Displacement.y;
        }
        else
        {
            simulated_displacement_y = sin(osc.Counter) * osc.Displacement.y;
        }
        float simulated_position_x = osc.Origin.x + simulated_displacement_x;
        float simulated_position_y = osc.Origin.y + simulated_displacement_y;
        float diff_x = simulated_position_x - pos.Value.x;
        float diff_y = simulated_position_y - pos.Value.y;

        vel.Value = { diff_x, diff_y };
        entity_manager->set_component_data(e, move(vel));
        entity_manager->set_component_data(e, move(osc));
    }
}
