#include "room_system.hpp"

using namespace std;

void RoomSystem::init()
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
}

void RoomSystem::update(float elapsed_sec)
{
    auto players = m_query->get_entities();
	auto enemies = m_enemies_query->get_entities();

    for (const auto& player : players)
    {
        auto position_comp = entity_manager->get_component_data<PositionComponent>(player);
        vec2 position = position_comp.Value;

        // Check if we won the game
        cout << position.y << endl;
        if (position.y < 360)
        {
            SceneService::getInstance()->setScene(CONGRATS);
        }

        RoomService& rs = RoomService::get_instance();

        int current_room_id = rs.room_id(position);
        if (current_room_id == -1)
        {
			//destory all enemies
			for (auto& e : enemies)
			{
				if (entity_manager->is_valid_entity(e)) {
					entity_manager->destroy_entity(e);
				}
			}
            event_manager->post(move(PlayerDeathEvent()));
        }
        else if (current_room_id != rs.get_current_room().id)
        {
			// destory enemies not in player's new room
			for (auto& e : enemies)
			{
				auto enemy_position_comp = entity_manager->get_component_data<PositionComponent>(e);
				int enemy_room_id = rs.room_id(enemy_position_comp.Value);
				if (enemy_room_id != current_room_id) {
					if (entity_manager->is_valid_entity(e)) {
						entity_manager->destroy_entity(e);
					}
				}
			}
            event_manager->post(move(RoomChangeEvent(rs.get_room(current_room_id))));
        }
    }
}
