#include "ECS/Systems/upgrade_system.hpp"

using namespace std;

void UpgradeSystem::init()
{
    EntityQuery upgrade_query =
        EntityQuery().all(3,
                          ComponentType<UpgradeComponent>::get_id(),
                          ComponentType<AABBComponent>::get_id(),
                          ComponentType<PositionComponent>::get_id());
    m_upgrade_query = make_shared<EntityQuery>(move(upgrade_query));
    add_query(m_upgrade_query);

    EntityQuery player_query =
        EntityQuery().all(2,
                          ComponentType<PlayerComponent>::get_id(),
                          ComponentType<PositionComponent>::get_id());
    m_player_query = make_shared<EntityQuery>(move(player_query));
    add_query(m_player_query);
}

void UpgradeSystem::update(float elapsed_sec)
{
    auto player = m_player_query->get_entities();
    auto upgrades = m_upgrade_query->get_entities();

    for (const auto& p : player)
    {
        auto player_position = entity_manager->get_component_data<PositionComponent>(p);
        auto player_aabb = entity_manager->get_component_data<AABBComponent>(p);

        for (const auto& c : upgrades)
        {
            if (!entity_manager->is_valid_entity(c))
            {
                continue;
            }
            auto upgrade = entity_manager->get_component_data<UpgradeComponent>(c);
            auto upgrade_position = entity_manager->get_component_data<PositionComponent>(c);
            auto upgrade_aabb = entity_manager->get_component_data<AABBComponent>(c);

            if (Common::are_colliding(player_position.Value.x, player_position.Value.y, player_aabb.HalfSize.x, player_aabb.HalfSize.y,
                                      upgrade_position.Value.x, upgrade_position.Value.y, upgrade_aabb.HalfSize.x, upgrade_aabb.HalfSize.y))
            {
                event_manager->post(UpgradeUnlockedEvent(upgrade.upgrade_type));
                entity_manager->destroy_entity(c);
            }
        }
    }
}