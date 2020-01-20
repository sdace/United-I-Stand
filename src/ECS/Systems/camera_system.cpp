#include "camera_system.hpp"

void CameraSystem::init()
{
    EntityQuery query = EntityQuery().all(1, ComponentType<PlayerComponent>::get_id());
    m_player_query = make_shared<EntityQuery>(move(query));
    add_query(m_player_query);
}

void CameraSystem::update(float elapsed_sec)
{
    auto entities = m_player_query->get_entities();
    if (entities.size() > 1) {
        std::cout << "CAMERASYSTEM::UPDATE: ERROR: found >1 player components" << std::endl;
    }
    if (entities.size() == 0) {
        std::cout << "CAMERASYSTEM::UPDATE: ERROR: found 0 player components" << std::endl;
    }
    for (const auto& e : entities)
    {
        PositionComponent pcomp = entity_manager->get_component_data<PositionComponent>(e);
        GUnit::get_instance().set_camera_position(pcomp.Value);
    }
}