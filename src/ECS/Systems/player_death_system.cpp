#include "ECS/Systems/player_death_system.hpp"

using namespace std;

void PlayerDeathSystem::init()
{
    EntityQuery query =
        EntityQuery()
        .all(2,
             ComponentType<PlayerComponent>::get_id(),
             ComponentType<PositionComponent>::get_id())
        .none(1,
              ComponentType<DeathComponent>::get_id());

    m_query = make_shared<EntityQuery>(move(query));
    add_query(m_query);

	EntityQuery enemies_query =
		EntityQuery().all(2,
			ComponentType<AABBComponent>::get_id(),
			ComponentType<EnemyComponent>::get_id());
	m_enemies_query = make_shared<EntityQuery>(move(enemies_query));
	add_query(m_enemies_query);

    auto player_death_handler = bind(&PlayerDeathSystem::player_death_handler, this, placeholders::_1);
    event_manager->add_handler<PlayerDeathEvent>(player_death_handler);
}

void PlayerDeathSystem::update(float elapsed_sec) {}

void PlayerDeathSystem::player_death_handler(PlayerDeathEvent ev)
{
    auto players = m_query->get_entities();
	auto enemies = m_enemies_query->get_entities();

    for (const auto& player : players)
    {
        auto position = entity_manager->get_component_data<PositionComponent>(player);

        Entity last_checkpoint = CheckpointService::get_instance().last_checkpoint();
        if (entity_manager->is_valid_entity(last_checkpoint))
        {
            auto respawn_position = entity_manager->get_component_data<PositionComponent>(last_checkpoint);
            entity_manager->set_component_data(player, move(respawn_position));
        }
        else
        {
            vec2 respawn_position = CheckpointService::get_instance().get_starting_position();
            entity_manager->set_component_data(player, move(PositionComponent(respawn_position)));
        }

        entity_manager->add_component_data(player, move(DeathComponent()));

    }

    SoundPlayer::get_instance().playsound("debug_off.wav");
    event_manager->post(PlayerRespawnEvent());
}