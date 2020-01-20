#include "ECS/Systems/checkpoint_system.hpp"

using namespace std;

void CheckpointSystem::init()
{
    EntityQuery checkpoint_query =
        EntityQuery().all(3,
                          ComponentType<CheckpointComponent>::get_id(),
                          ComponentType<AABBComponent>::get_id(),
                          ComponentType<PositionComponent>::get_id());
    m_checkpoint_query = make_shared<EntityQuery>(move(checkpoint_query));
    add_query(m_checkpoint_query);

    EntityQuery player_query =
        EntityQuery().all(2,
                          ComponentType<PlayerComponent>::get_id(),
                          ComponentType<PositionComponent>::get_id());
    m_player_query = make_shared<EntityQuery>(move(player_query));
    add_query(m_player_query);

    m_frame_duration = 10;
    m_frame_timer = 0;
}

void CheckpointSystem::update(float elapsed_sec)
{
    update_checkpoints();
    animate_checkpoints();
}

void CheckpointSystem::update_checkpoints()
{
    auto player = m_player_query->get_entities();
    auto checkpoints = m_checkpoint_query->get_entities();

    for (const auto& p : player)
    {
        auto player_position = entity_manager->get_component_data<PositionComponent>(p);
        auto player_aabb = entity_manager->get_component_data<AABBComponent>(p);

        for (const auto& c : checkpoints)
        {
            auto checkpoint = entity_manager->get_component_data<CheckpointComponent>(c);

            if (checkpoint.Activated)
            {
                continue;
            }

            auto checkpoint_position = entity_manager->get_component_data<PositionComponent>(c);
            auto checkpoint_aabb = entity_manager->get_component_data<AABBComponent>(c);

            if (Common::are_colliding(player_position.Value.x, player_position.Value.y, player_aabb.HalfSize.x, player_aabb.HalfSize.y,
                                      checkpoint_position.Value.x, checkpoint_position.Value.y, checkpoint_aabb.HalfSize.x, checkpoint_aabb.HalfSize.y))
            {
                event_manager->post(ActivateCheckpointEvent(c));
            }
        }
    }
}

void CheckpointSystem::animate_checkpoints()
{
    m_frame_timer += 1;
    if (m_frame_timer >= m_frame_duration)
    {
        GUnit::get_instance().advance_frame("active_checkpoint");
        m_frame_timer = 0;
    }
}